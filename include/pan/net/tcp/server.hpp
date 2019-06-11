#ifndef __PAN_NET_TCP_SERVER_HPP__
#define __PAN_NET_TCP_SERVER_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

/*
 * Don't really need to manage sessions here, 
 * In most situation, handler needs to interact with session_pool, 
 * 
 * In old implementation, we need to pass each sessions into hander.
 * Then, we need to manage two associated session pools at the same time.
 * And have to do a bit more steps to synchronize two connection pools. 
 * That implementation was ugly.
 * 
 * So far, any session pool here or in handler is just an option.
 * Two session pool here and in handler are totaly individual.
 * And be synchronized by session's interal member function start/stop().
 */
template <typename Handler>
class server : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef acceptor<handler_type> acceptor_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::key_type key_type;
    typedef typename session_type::pointer session_ptr;
    typedef pan::map<key_type, session_ptr> pool_type;

    explicit server(uint16_t port, bool use_thread = false)
        : io_context_()
        , handler_()
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
        pool_.insert(session->id(), session);
    }

    void close_session(session_ptr session)
    {
        pool_.remove(session->id());
    }

protected:
    boost::asio::io_context io_context_;
    handler_type handler_;
    acceptor_type acceptor_;
    // FIXME: pool needs to be optimized.
    pool_type pool_;
    std::thread thread_;
    bool use_thread_;
    
};

}}}

#endif // __PAN_NET_TCP_SERVER_HPP__