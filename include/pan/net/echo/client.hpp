#ifndef __PAN_NET_ECHO_CLIENT_HPP__
#define __PAN_NET_ECHO_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/client_handler.hpp>

namespace pan { namespace net { namespace echo {

using client = tcp::basic_client<echo::client_handler<tcp::session> >;

}}}

#endif // __PAN_NET_ECHO_CLIENT_HPP__