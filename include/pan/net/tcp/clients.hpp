#ifndef __PAN_NET_TCP_CLIENTS_HPP__
#define __PAN_NET_TCP_CLIENTS_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {
    
template <typename Handler>
class clients : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef connector<handler_type> connector_type;

    explicit clients(const std::string& host, uint16_t port)
        : io_context_()
        , handler_()
        , connector_(io_context_, handler_)
    {
        connector_.connect(host, port);
        thread_ = std::thread([this]() { io_context_.run(); });
    }

    virtual ~clients()
    {
        thread_.join();
    }

    handler_type& handler()
    {
        return handler_;
    }

    const handler_type& handler() const
    {
        return handler_;
    }

    void connect(const std::string& host, uint16_t port)
    {
        connector_.connect(host, port);
    }

protected:
    boost::asio::io_context io_context_;
    handler_type handler_;
    connector_type connector_;
    std::thread thread_;

};

}}}

#endif // __PAN_NET_TCP_CLIENTS_HPP__