#ifndef __PAN_NET_TCP_BASIC_SERVER_HPP__
#define __PAN_NET_TCP_BASIC_SERVER_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/server.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class basic_server : public noncopyable {
    using handler_type = Handler;

public:
    explicit basic_server(std::uint16_t port = 8888)
        : io_context_()
        , handler_()
        , server_(io_context_, port, handler_)
    {

    }
    
    virtual ~basic_server()
    {
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

    void run()
    {
        thread_ = std::thread([this]() { io_context_.run(); });
    }
    
private:
    boost::asio::io_context io_context_;
    handler_type handler_;
    server server_;
    std::thread thread_;
    
};

}}}

#endif // __PAN_NET_TCP_BASIC_SERVER_HPP__