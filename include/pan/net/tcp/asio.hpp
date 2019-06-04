#ifndef __PAN_NET_TCP_ASIO_HPP__
#define __PAN_NET_TCP_ASIO_HPP__

#include <cstdint>
#include <string>
#include <boost/asio.hpp>
#include <pan/base.hpp>

namespace pan { namespace net { namespace asio {

template <typename SessionPred>
inline static void accept(boost::asio::ip::tcp::acceptor& acceptor, SessionPred pred)
{
    acceptor.async_accept(
        [pred, &acceptor](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            pred(std::move(socket));
        } else {
            LOG_ERROR("asio.accept.error: %s", ec.message().c_str());
        }
        pan::net::asio::accept(acceptor, pred);
    });
}

template <typename ResolvePred>
inline static void resolve(boost::asio::ip::tcp::resolver& resolver, const std::string& host, const std::string& port, ResolvePred pred)
{
    resolver.async_resolve(boost::asio::ip::tcp::v4(), host, port,
        [pred](boost::system::error_code ec, const boost::asio::ip::tcp::resolver::results_type& endpoints) {
        if (!ec) {
            pred(endpoints);
        } else {
            LOG_ERROR("asio.resolve.error: %s", ec.message().c_str());
        }
    });
}

template <typename SessionPred>
inline static void connect(boost::asio::io_context& ios, const boost::asio::ip::tcp::resolver::results_type& endpoints, SessionPred pred)
{
    auto sock = std::make_shared<boost::asio::ip::tcp::socket>(ios);
    boost::asio::async_connect(*sock, endpoints,
        [pred, sock](boost::system::error_code ec, boost::asio::ip::tcp::endpoint ep) {
        if (!ec) {
            pred(std::move(*sock));
        } else {
            LOG_ERROR("asio.connect.error: %s", ec.message().c_str());
        }
    });
}

template <typename ReadPred, typename FailurePred>
inline static void read(boost::asio::ip::tcp::socket& socket, void* data, size_t size, ReadPred pred, FailurePred failure)
{
    socket.async_read_some(boost::asio::buffer(data, size),
        [pred, failure, data, size](boost::system::error_code ec, size_t size) {
        if (!ec) {
            pred(data, size);
        } else if (ec != boost::asio::error::operation_aborted) {
            LOG_ERROR("asio.read.error: %s", ec.message().c_str());
            failure();
        }
    });
}

template <typename WritePred, typename FailurePred>
inline static void write(boost::asio::ip::tcp::socket& socket, const void* data, size_t size, WritePred pred, FailurePred failure)
{
    auto buffer = std::make_shared<std::string>((const char*)data, (const char*)data + size);
    boost::asio::async_write(socket, boost::asio::buffer(buffer->data(), buffer->size()),
        [pred, failure, buffer](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
            pred(buffer->data(), buffer->size());
        } else if (ec != boost::asio::error::operation_aborted) {
            LOG_ERROR("asio.write.error: %s", ec.message().c_str());
            failure();
        }
    });
}

inline static void close(boost::asio::ip::tcp::socket& socket)
{
    if (socket.is_open()) {
        socket.shutdown(boost::asio::socket_base::shutdown_both);
        socket.close();
    }
}

}}}

#endif // __PAN_NET_TCP_ASIO_HPP__