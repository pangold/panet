#ifndef __PAN_NET_TCP_CLIENT_IMPL_HPP__
#define __PAN_NET_TCP_CLIENT_IMPL_HPP__

#include <pan/net/tcp/client.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/connector.hpp>

namespace pan { namespace net { namespace tcp {
    
template <typename Handler>
client<Handler>::client(const std::string& host, uint16_t port)
    : io_context_()
    , handler_()
    , connector_(io_context_, host, std::to_string(port), handler_)
{
    //if (connector_.connect(host, port)) {
    thread_ = std::thread([this]() { io_context_.run(); });
    //}
}

template <typename Handler>
client<Handler>::~client()
{
    //connector_.close();
    if (thread_.joinable()) {
        thread_.join();
    }
}

}}}

#endif // __PAN_NET_TCP_CLIENT_IMPL_HPP__