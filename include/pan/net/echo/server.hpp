#ifndef __PAN_NET_ECHO_SERVER_HPP__
#define __PAN_NET_ECHO_SERVER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/server_handler.hpp>

namespace pan { namespace net { namespace echo {

class server : public tcp::server<echo::server_handler> {
    typedef tcp::server<echo::server_handler> _Mybase;
public:
    server(uint16_t port)
        : _Mybase(port)
    { }
};

}}}

#endif // __PAN_NET_ECHO_SERVER_HPP__