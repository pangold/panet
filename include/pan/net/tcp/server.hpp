#ifndef __PAN_NET_TCP_SERVER_HPP__
#define __PAN_NET_TCP_SERVER_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

template <typename Handler>
class server : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef acceptor<handler_type> acceptor_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::key_type key_type;
    typedef typename session_type::pointer session_ptr;
    typedef pan::map<key_type, session_ptr> pool_type;

    explicit server(uint16_t port = 8888)
        : io_context_()
        , handler_()
        , acceptor_(io_context_, port, handler_)
    {
        auto pred = std::bind(&server::new_session, this, std::placeholders::_1);
        acceptor_.register_session_callback(pred);
    }

    virtual ~server() 
    {
        pool_.clear();
    }

    void run() 
    { 
        io_context_.run();
    }

protected:
    void new_session(session_ptr session)
    {
        pool_.insert(session->id(), session);
    }

protected:
    boost::asio::io_context io_context_;
    handler_type handler_;
    acceptor_type acceptor_;
    pool_type pool_;
    
};

}}}

#endif // __PAN_NET_TCP_SERVER_HPP__