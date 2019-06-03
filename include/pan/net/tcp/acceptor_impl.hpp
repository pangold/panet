#ifndef __PAN_NET_TCP_ACCEPTOR_IMPL_HPP__
#define __PAN_NET_TCP_ACCEPTOR_IMPL_HPP__

#include <pan/net/tcp/acceptor.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace tcp {

acceptor::acceptor(boost::asio::io_context& io_context, std::uint16_t port, handler_type& handler)
    : io_context_(io_context)
    , acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    , handler_(handler)
{
    accept_();
}

acceptor::~acceptor()
{
    io_context_.post([this]() { acceptor_.close(); });
}

void acceptor::accept_()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            static session::key_type id = 0;
            std::make_shared<session>(++id, std::move(socket), handler_)->start();
        } else {
            LOG_ERROR("tcp.acceptor.accept.error: %s", ec.message().c_str());
        }
        accept_();
    });
}

}}}

#endif // __PAN_NET_TCP_ACCEPTOR_IMPL_HPP__
