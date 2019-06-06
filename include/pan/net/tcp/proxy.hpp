#ifndef __PAN_NET_TCP_PROXY_HPP__
#define __PAN_NET_TCP_PROXY_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {

/* Basic Handler's member function required(see the handler_base<>): 
 * ------------------------------------------------------------------
 * void on_session_start(session_ptr);
 * void on_session_stop(session_ptr);
 * void on_write(session_ptr, const void*, std::size_t);
 * std::size_t on_message(session_ptr, const void*, std::size_t);
 * ------------------------------------------------------------------
 * You can directly inherit handler_base<Inherit>, 
 * Or, define the same named member functions.
 * ==================================================================
 * ProxyHandler's member function required(see the proxy_handler_base<>): 
 * ------------------------------------------------------------------
 * void set_associated_handler(proxy_handler_base<Target, Inherit>* association);
 * void set_connection_op(connect_op_type op);
 * void run();
 * ------------------------------------------------------------------
 * You can directly inherit proxy_handler_base<Inherit, Target>, 
 * Or, define the same named member functions.
 * ==================================================================
 */
template <typename UpstreamHandler, 
    typename DownstreamHandler>
class proxy : public server<DownstreamHandler> {
    typedef server<DownstreamHandler> _Mybase;
public:
    typedef UpstreamHandler upstream_handler_type;
    typedef connector<upstream_handler_type> connector_type;

    explicit proxy(std::uint16_t port)
        : _Mybase(port, false)
        , upstream_handler_()
        , connector_(io_context_, upstream_handler_)
    {
        using namespace std::placeholders;
        auto pred = std::bind(&connector_type::connect, &connector_, _1, _2);
        upstream_handler_.set_associated_handler(&handler_);
        upstream_handler_.set_connection_op(pred);
    }

    void run()
    {
        upstream_handler_.run();
        _Mybase::run();
    }

protected:
    upstream_handler_type upstream_handler_;
    connector_type connector_;
    
};

}}}


#endif // __PAN_NET_TCP_PROXY_HPP__