#ifndef __PAN_NET_RPC_SERVER_HPP__
#define __PAN_NET_RPC_SERVER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/server_handler.hpp>

namespace pan { namespace net { namespace rpc {

using server = tcp::basic_server<rpc::server_handler<tcp::session> >;

}}}

#endif // __PAN_NET_RPC_SERVER_HPP__