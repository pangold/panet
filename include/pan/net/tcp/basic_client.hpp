#ifndef __PAN_NET_TCP_BASIC_CLIENT_HPP__
#define __PAN_NET_TCP_BASIC_CLIENT_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/client.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class basic_client : public noncopyable {
    using handler_type = Handler;

public:
    explicit basic_client(const std::string& host = "localhost", uint16_t port = 8888)
        : io_context_()
        , handler_()
        , client_(io_context_, handler_)
    {
        if (client_.connect(host, port)) {
            thread_ = std::thread([this]() { io_context_.run(); });
        }
    }

    virtual ~basic_client()
    {
        client_.close();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    handler_type& handler()
    {
        return handler_;
    }

    const handler_type& handler() const
    {
        return handler_;
    }

private:
    boost::asio::io_context io_context_;
    handler_type handler_;
    client client_;
    std::thread thread_;

};

}}}


#endif // __PAN_NET_TCP_BASIC_CLIENT_HPP__