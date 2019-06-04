#ifndef __PAN_NET_TCP_SESSION_HPP__
#define __PAN_NET_TCP_SESSION_HPP__

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>
#include <pan/base.hpp>

/* This tcp will not change, the only variable thing is handler.
 * attach handler to tcp module, 
 * if protobuf, attach protobuf_handler to tcp module. 
 * and attach protobuf messages to protobuf_handler.
 * think about combine the real business logic(concrete protobuf message) to handlers.
 *
 * rpc::requester::on_message(std::shared_ptr<Pango::Rpc::Request> message)
 * rpc::responder::on_message(std::shared_ptr<Pango::Rpc::Respond> message)
 * class rpc_handler : public protobuf_handler {};
 * rpc_handler::register_protobuf_message(rpc::requester::on_message, requester_, _1, _2);
 * rpc_handler::register_protobuf_message(rpc::responder::on_message, responder_, _1, _2);
 *
 * combination like these: 
 * tcp::server<rpc_handler>
 * tcp::client<rpc_handler>
 * then, done.....
 */

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class session 
    : public std::enable_shared_from_this<session<Handler>>
    , public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef pan::buffer<1024> buffer_type;
    typedef std::size_t key_type;
    typedef session<handler_type> value_type;
    typedef std::shared_ptr<value_type> pointer;
    typedef void* content_type;

    session(boost::asio::ip::tcp::socket socket, handler_type& handler)
        : id_(value_type::make_key())
        , socket_(std::move(socket))
        , read_buffer_()
        , handler_(handler)
    {

    }

    virtual ~session()
    {
        stop();
    }

    key_type id() const 
    {
        return id_; 
    }

    template <typename T>
    void set_content(T* content)
    {
        content_ = reinterpret_cast<void*>(content);
    }

    template <typename T>
    T* content() const
    {
        return reinterpret_cast<T*>(content_);
    }

    void start()
    {
        read();
    }

    void stop()
    {
        pan::net::asio::close(socket_);
    }

    void write(const void* data, size_t size)
    {
        auto self = shared_from_this();
        auto success = [this, self](const void* d, size_t s) { handler_.on_write(self, d, s); };
        auto failure = [self]() { self->stop(); };
        pan::net::asio::write(socket_, data, size, success, failure);
    }

    void read()
    {
        auto self = shared_from_this();
        auto success = [self](const void* d, size_t s) { self->received(d, s); self->read(); };
        auto failure = [self]() { self->stop(); };
        pan::net::asio::read(socket_, read_buffer_.free_data(), read_buffer_.free_size(), success, failure);
    }

private:
    void received(const void* data, size_t size)
    {
        read_buffer_.push(size);
        auto len = handler_.on_message(this->shared_from_this(), read_buffer_.data(), read_buffer_.size());
        if (len > 0) {
            read_buffer_.pop(std::min(len, size));
        }
        else if (read_buffer_.full()) {
            read_buffer_.lengthen();
        }
    }

    static key_type make_key()
    {
        static key_type id = 0;
        return ++id;
    }

protected:
    key_type id_;
    boost::asio::ip::tcp::socket socket_;
    buffer_type read_buffer_;
    handler_type& handler_;
    content_type content_;

};

}}}

#endif // __PAN_NET_TCP_SESSION_HPP__
