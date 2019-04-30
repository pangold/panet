#ifndef __PAN_NET_RPC_CLIENT_HPP__
#define __PAN_NET_RPC_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/client_handler.hpp>

namespace pan { namespace net { namespace rpc {

using client = tcp::basic_client<rpc::client_handler<tcp::session> >;

}}}

#endif // __PAN_NET_RPC_CLIENT_HPP__