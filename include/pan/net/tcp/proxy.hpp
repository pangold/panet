#ifndef __PAN_NET_TCP_PROXY_HPP__
#define __PAN_NET_TCP_PROXY_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

/* Think about how to match proxy-server(from end-clients) with proxy-client(to real services)
 * end-clients     could be many
 * services        could be many also
 * service-center  basically one, if multiples, one should be master, the others should be slaves.
 * 
 * conditions: 
 * one acceptor, accept connection from clients
 * many connectors, connect to servcies
 */

template <typename Handler>
class proxy : public pan::noncopyable {
public:
    typedef Handler handler_type;
    typedef acceptor<handler_type> acceptor_type;
    typedef connector<handler_type> connector_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::key_type key_type;
    typedef typename session_type::pointer session_ptr;
    // downstream connections, basically are end-clients
    typedef pan::map<key_type, session_ptr> downstream_pool_type;
    // upstream connections, basically are service providers/dependent services.
    typedef pan::map<std::string, session_ptr> upstream_pool_type;

    explicit proxy(std::uint16_t port = 8888)
        : io_context_()
        , handler_()
        , acceptor_(io_context_, port, handler_)
        , connector_(io_context_, handler_)
    {
        using namespace std::placeholders;
        connector_.register_new_session_callback(std::bind(&proxy::new_upstream_session, this, _1));
        connector_.register_close_session_callback(std::bind(&proxy::close_upstream_session, this, _1));
        acceptor_.register_new_session_callback(std::bind(&proxy::new_downstream_session, this, _1));
        acceptor_.register_close_session_callback(std::bind(&proxy::close_downstream_session, this, _1));
    }

    virtual ~proxy()
    {
        stop();
    }

    // connect to upstream services outside
    void connect(const std::string& host, uint16_t port)
    {
        connector_.connect(host, port);
    }

    void run()
    {
        thread_ = std::thread([this]() { io_context_.run(); });
    }

    void stop()
    {
        thread_.join();
    }

protected:
    void new_upstream_session(session_ptr session)
    {
        LOG_INFO("tcp.proxy.new.upstream.session");
        upstream_pool_.insert(session->to_string(), session);
    }

    void new_downstream_session(session_ptr session)
    {
        LOG_INFO("tcp.proxy.new.downstream.session");
        downstream_pool_.insert(session->id(), session);
    }

    void close_upstream_session(session_ptr session)
    {
        upstream_pool_.remove(session->to_string());
    }

    void close_downstream_session(session_ptr session)
    {
        downstream_pool_.remove(session->id());
    }

protected:
    boost::asio::io_context io_context_;
    handler_type handler_;
    acceptor_type acceptor_;
    connector_type connector_;
    downstream_pool_type downstream_pool_;
    upstream_pool_type upstream_pool_;
    std::thread thread_;
    
};

}}}


#endif // __PAN_NET_TCP_PROXY_HPP__