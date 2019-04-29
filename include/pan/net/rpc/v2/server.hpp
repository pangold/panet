#ifndef __PAN_NET_RPC_SERVER_V2_HPP__
#define __PAN_NET_RPC_SERVER_V2_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/v2/server_handler.hpp>

namespace pan { namespace net { namespace rpc { namespace v2 {

using server = tcp::basic_server<rpc::v2::server_handler<tcp::session> >;

}}}}

#endif // __PAN_NET_RPC_SERVER_V2_HPP__