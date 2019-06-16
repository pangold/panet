#ifndef __PAN_NET_TCP_CLIENTS_HPP__
#define __PAN_NET_TCP_CLIENTS_HPP__

#include <pan/net/tcp/bridge.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/session_pool.hpp>
#include <pan/net/tcp/connector.hpp>

namespace pan { namespace net { namespace tcp {
    
template <typename Handler>
class clients : public pan::noncopyable {
    friend typename Handler;
public:
    typedef Handler handler_type;
    typedef connector<handler_type> connector_type;
    typedef session<handler_type> session_type;
    typedef session_pool<handler_type> session_pool_type;
    typedef typename session_type::pointer session_ptr;

    explicit clients(const std::string& host = std::string(), uint16_t port = 0)
        : io_context_()
        , pool_()
        , handler_(pool_)
        , connector_(io_context_, handler_)
    {
        using namespace std::placeholders;
        connector_.register_new_session_callback(std::bind(&clients::new_session, this, _1));
        connector_.register_close_session_callback(std::bind(&clients::close_session, this, _1));
        if (!host.empty() && port != 0) connect(host, port);
    }

    virtual ~clients()
    {
        stop();
        if (thread_.joinable()) thread_.join();
    }

    void connect(const std::string& host, uint16_t port)
    {
        connector_.connect(host, port);
        if (!thread_.joinable()) {
            thread_ = std::thread([this]() { io_context_.run(); });
        }
    }

    void stop()
    {
        pool_.clear();
    }

protected:
    void new_session(session_ptr session)
    {
        pool_.insert(session);
    }

    void close_session(session_ptr session)
    {
        pool_.remove(session);
    }

protected:
    boost::asio::io_context io_context_;
    session_pool_type pool_;
    handler_type handler_;
    connector_type connector_;
    std::thread thread_;

};

}}}

#endif // __PAN_NET_TCP_CLIENTS_HPP__