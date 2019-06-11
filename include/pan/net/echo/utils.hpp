#ifndef __PAN_NET_ECHO_CLIENT_HPP__
#define __PAN_NET_ECHO_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/server_handler.hpp>
#include <pan/net/echo/client_handler.hpp>
#include <pan/net/echo/datagram_client.hpp>

namespace pan { namespace net { namespace echo {

typedef tcp::server<server_handler> server;
typedef tcp::client<client_handler> client;

}}}

#endif // __PAN_NET_ECHO_CLIENT_HPP__