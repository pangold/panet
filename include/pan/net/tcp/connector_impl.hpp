#ifndef __PAN_NET_TCP_CONNECTOR_IMPL_HPP__
#define __PAN_NET_TCP_CONNECTOR_IMPL_HPP__

#include <pan/net/tcp/connector.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace tcp {

connector::connector(boost::asio::io_context& io_context, const std::string& host, const std::string& port, handler_type& handler)
    : io_context_(io_context)
    , resolver_(io_context)
    , handler_(handler)
{
    resolve_(host, port);
}

connector::~connector()
{

}

void connector::resolve_(const std::string& host, const std::string& port)
{
    resolver_.async_resolve(boost::asio::ip::tcp::v4(), host, port,
        [this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type endpoints) {
        if (!ec) {
            connect_(endpoints);
        }
        else {
            LOG_ERROR("tcp.connector.resolve.error: %s", ec.message().c_str());
        }
    });
}

void connector::connect_(const boost::asio::ip::tcp::resolver::results_type& endpoints)
{
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
    boost::asio::async_connect(*socket, endpoints,
        [this](boost::system::error_code ec,
            boost::asio::ip::tcp::endpoint ep) {
        if (!ec) {

        }
        else {
            LOG_ERROR("tcp.connector.connect.error: %s", ec.message().c_str());
        }
    });
}

}}}

#endif // __PAN_NET_TCP_CONNECTOR_IMPL_HPP__
