#ifndef __PAN_NET_TCP_PROXY_HPP__
#define __PAN_NET_TCP_PROXY_HPP__

#include <boost/asio.hpp>
#include <pan/base.hpp>
#include <pan/net/tcp/session.hpp>
#include <pan/net/tcp/server.hpp>
#include <pan/net/tcp/client.hpp>

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
class proxy : public server<Handler> {
    typedef server<Handler> _Mybase;
public:
    typedef connector<handler_type> connector_type;
    typedef std::shared_ptr<connector_type> connector_ptr;

    explicit proxy(std::uint16_t port = 8888)
        : _Mybase(port)
    {

    }

    virtual ~proxy()
    {

    }

    void connect(const std::string& host = "localhost", uint16_t port = 8888)
    {
        connector_ = std::make_shared<connector_type>(io_context_, host, std::to_string(port), handler_);
        auto pred = std::bind(&proxy::new_upstream_session, this, std::placeholders::_1);
        connector_->register_session_callback(pred);
    }

protected:
    void new_upstream_session(session_ptr session)
    {
        session_ = session;
        // std::string temp("1234567890");
        // session_->write(temp.data(), temp.size());
    }

protected:
    connector_ptr connector_;
    session_ptr session_;
    
};

}}}


#endif // __PAN_NET_TCP_PROXY_HPP__