#ifndef __PAN_NET_TCP_SESSION_IMPL_HPP__
#define __PAN_NET_TCP_SESSION_IMPL_HPP__

#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace tcp {

session::session(key_type id, boost::asio::ip::tcp::socket socket, handler_type& handler)
    : id_(id)
    , socket_(std::move(socket))
    , read_buffer_()
    , handler_(handler)
{

}

session::~session()
{
    stop();
}

void session::start()
{
    handler_.on_session_start(this->shared_from_this());
    read_();
}

void session::stop()
{
    if (socket_.is_open()) {
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
        socket_.close();
        handler_.on_session_stop(this->shared_from_this());
    }
}

void session::write(const void* data, std::size_t size)
{
    write_((const char*)data, size);
}

void session::write(const std::string& data)
{
    write_(data.data(), data.size());
}

void session::received_(std::size_t size)
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

void session::read_()
{
    auto self = this->shared_from_this();
    socket_.async_read_some(boost::asio::buffer(read_buffer_.free_data(), read_buffer_.free_size()),
        [this, self](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
            received_(size);
            read_();
        }
        else if (ec != boost::asio::error::operation_aborted) {
            LOG_ERROR("tcp.session.read.error: %s", ec.message().c_str());
            stop();
        }
    });
}

void session::write_(const char* buffer, std::size_t size)
{
    auto self = this->shared_from_this();
    auto data = std::make_shared<std::string>((buffer, buffer + size));
    boost::asio::async_write(socket_, boost::asio::buffer(data->data(), data->size()),
        [this, self, data](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
            handler_.on_write(self, data->data(), data->size());
        }
        else if (ec != boost::asio::error::operation_aborted) {
            LOG_ERROR("tcp.session.write.error: %s", ec.message().c_str());
            stop();
        }
    });
}

}}}

#endif // __PAN_NET_TCP_SESSION_IMPL_HPP__
