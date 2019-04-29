#ifndef __PAN_NET_RPC_CLIENT_V2_HPP__
#define __PAN_NET_RPC_CLIENT_V2_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/v2/client_handler.hpp>

namespace pan { namespace net { namespace rpc { namespace v2 {

using client = tcp::basic_client<rpc::v2::client_handler<tcp::session> >;

}}}}

#endif // __PAN_NET_RPC_CLIENT_V2_HPP__