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
    typedef std::function<void(pointer)> start_callback_type;
    typedef std::function<void(pointer)> close_callback_type;

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
    
    // format: "ip:port"
    std::string to_string() const
    {
        assert(socket_.is_open());
        auto& ep = socket_.remote_endpoint();
        return ep.address().to_string() + ":" + std::to_string(ep.port());
    }

    std::string ip() const
    {
        assert(socket_.is_open());
        return socket_.remote_endpoint().address().to_string();
    }

    uint16_t port() const
    {
        assert(socket_.is_open());
        return socket_.remote_endpoint().port();
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

    void register_start_callback(start_callback_type cb)
    {
        start_callback_ = std::move(cb);
    }

    void register_close_callback(close_callback_type cb)
    {
        close_callback_ = std::move(cb);
    }

    void start()
    {
        LOG_INFO("tcp.session.start: id<%d>:%s", id(), to_string().c_str());
        // Here are two options to callback session started state.
        if (start_callback_) start_callback_(shared_from_this());
        handler_.on_session_start(shared_from_this());
        read();
    }

    void stop()
    {
        if (!socket_.is_open()) return;
        LOG_INFO("tcp.session.stop: id<%d>:%s", id(), to_string().c_str());
        // Here are two options to callback session closed state.
        // Note!!!! No session->stop() invokement within callback. 
        // Or it will cause dead loop, and then crash...
        if (close_callback_) close_callback_(shared_from_this());
        handler_.on_session_stop(shared_from_this());
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
        socket_.close();
    }

    void write(const void* data, size_t size)
    {
        auto self = shared_from_this();
        auto success = [this, self](const void* d, size_t s) { handler_.on_write(self, d, s); };
        auto failure = [self]() { self->stop(); };
        pan::net::asio::write(socket_, data, size, success, failure);
    }

private:
    void read()
    {
        auto self = shared_from_this();
        auto success = [self](const void* d, size_t s) { self->received(d, s); self->read(); };
        auto failure = [self]() { self->stop(); };
        pan::net::asio::read(socket_, read_buffer_.free_data(), read_buffer_.free_size(), success, failure);
    }

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
    start_callback_type start_callback_;
    close_callback_type close_callback_;

};

}}}

#endif // __PAN_NET_TCP_SESSION_HPP__
