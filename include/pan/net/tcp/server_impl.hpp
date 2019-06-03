#ifndef __PAN_NET_TCP_SERVER_IMPL_HPP__
#define __PAN_NET_TCP_SERVER_IMPL_HPP__

#include <pan/net/tcp/server.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/acceptor.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
server<Handler>::server(std::uint16_t port)
    : io_context_()
    , handler_()
    , acceptor_(io_context_, port, handler_)
{

}

template <typename Handler>
server<Handler>::~server()
{
    if (thread_.joinable()) {
        thread_.join();
    }
}

template <typename Handler>
void server<Handler>::run()
{
    thread_ = std::thread([this]() { io_context_.run(); });
}

}}}

#endif // __PAN_NET_TCP_SERVER_IMPL_HPP__