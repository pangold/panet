#ifndef __PAN_NET_TCP_SERVER_HPP__
#define __PAN_NET_TCP_SERVER_HPP__

#include <memory>
#include <cstdint>
#include <boost/asio.hpp>
#include <net/base/log.hpp>
#include <net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

class server {
public:
    using handler_type = session::handler_type;

    server(const server&) = delete;
    server& operator=(const server&) = delete;

    server(boost::asio::io_context& io_context, std::uint16_t port, handler_type& handler)
        : io_context_(io_context)
        , acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
        , handler_(handler)
    {
        accept();
    }

    virtual ~server()
    {
        io_context_.post([this]() { acceptor_.close(); });
    }

protected:
    void accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                static session::key_type id = 0;
                std::make_shared<session>(0, std::move(socket), handler_)->start();
            } else {
                LOG_ERROR("tcp.server.accept.error: %s", ec.message().c_str());
            }
            accept();
        });
    }

protected:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    handler_type& handler_;

};

}}}

#endif // __PAN_NET_TCP_SERVER_HPP__
