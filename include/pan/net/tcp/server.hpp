#ifndef __PAN_NET_TCP_SERVER_HPP__
#define __PAN_NET_TCP_SERVER_HPP__

#include <pan/net/tcp/bridge.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/session_pool.hpp>
#include <pan/net/tcp/acceptor.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class server : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef acceptor<handler_type> acceptor_type;
    typedef session<handler_type> session_type;
    typedef session_pool<handler_type> session_pool_type;
    typedef typename session_type::pointer session_ptr;

    explicit server(uint16_t port, bool use_thread = false)
        : io_context_()
        , pool_()
        , handler_(pool_)
        , acceptor_(io_context_, port, handler_)
        , use_thread_(use_thread)
    {
        using namespace std::placeholders;
        acceptor_.register_new_session_callback(std::bind(&server::new_session, this, _1));
        acceptor_.register_close_session_callback(std::bind(&server::close_session, this, _1));
    }

    virtual ~server() 
    {
        if (use_thread_ && thread_.joinable()) thread_.join();
        pool_.clear();
    }

    void run() 
    { 
        if (!use_thread_) io_context_.run();
        else thread_ = std::thread([this]() { io_context_.run(); });
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
    acceptor_type acceptor_;
    std::thread thread_;
    bool use_thread_;
    
};

}}}

#endif // __PAN_NET_TCP_SERVER_HPP__