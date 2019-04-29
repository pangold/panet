#ifndef __PAN_NET_ECHO_SERVER_HPP__
#define __PAN_NET_ECHO_SERVER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/server_handler.hpp>

namespace pan { namespace net { namespace echo {

using server = tcp::basic_server<echo::server_handler<tcp::session> >;

}}}

#endif // __PAN_NET_ECHO_SERVER_HPP__