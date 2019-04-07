#ifndef __PAN_NET_TCP_SESSION_HPP__
#define __PAN_NET_TCP_SESSION_HPP__

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>
#include <net/base/log.hpp>
#include <net/tcp/buffer.hpp>
#include <net/handler_base.hpp>

namespace pan { namespace net { namespace tcp {

class session 
    : public std::enable_shared_from_this<session> {
public:
    using handler_type = handler_base<session>;
    using buffer_type = buffer<1024>;
    using key_type = std::size_t;
    using pointer = std::shared_ptr<session>;

public:
    session(const session&) = delete;
    session& operator=(const session&) = delete;

    session(key_type id, boost::asio::ip::tcp::socket socket, handler_type& handler)
        : id_(id)
        , socket_(std::move(socket))
        , read_buffer_()
        , handler_(handler)
    {

    }

    virtual ~session()
    {
        stop();
    }

    void start()
    {
        handler_.on_start(this->shared_from_this());
        do_read();
    }

    void stop()
    {
        if (socket_.is_open()) {
            socket_.shutdown(boost::asio::socket_base::shutdown_both);
            socket_.close();
            handler_.on_stop(this->shared_from_this());
        }
    }

    void write(const void* data, std::size_t size)
    {
        do_write(std::make_shared<std::vector<char>>(
            (const char*)data, (const char*)data + size));
    }

private:
    void do_received(std::size_t size)
    {
        read_buffer_.push(size);
        auto len = handler_.on_message(this->shared_from_this(), read_buffer_.data(), read_buffer_.size());
        if (len > 0) {
            read_buffer_.pop(std::min(len, size));
        } else if (read_buffer_.full()) {
            read_buffer_.lengthen();
        }
    }

    void do_read()
    {
        auto self(this->shared_from_this());
        socket_.async_read_some(boost::asio::buffer(read_buffer_.free_data(), read_buffer_.free_size()),
            [this, self](boost::system::error_code ec, std::size_t size) {
            if (!ec) {
                do_received(size);
                do_read();
            } else if (ec != boost::asio::error::operation_aborted) {
                LOG_ERROR("tcp.session.read.error: %s", ec.message().c_str());
                handler_.on_error(self, ec.message());
                stop();
            }
        });
    }

    void do_write(std::shared_ptr<std::vector<char>> data)
    {
        auto self(this->shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data->data(), data->size()),
            [this, self, data](boost::system::error_code ec, std::size_t size) {
            if (!ec) {
                handler_.on_wrote(self, data->data(), data->size());
            } else if (ec != boost::asio::error::operation_aborted) {
                LOG_ERROR("tcp.session.write.error: %s", ec.message().c_str());
                handler_.on_error(self, ec.message());
                stop();
            }
        });
    }

private:
    key_type id_;
    boost::asio::ip::tcp::socket socket_;
    buffer_type read_buffer_;
    handler_type& handler_;

};

}}}

#endif // __PAN_NET_TCP_SESSION_HPP__
