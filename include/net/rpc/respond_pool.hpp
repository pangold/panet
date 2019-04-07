#ifndef __PAN_NET_RPC_RESPOND_POOL_HPP__
#define __PAN_NET_RPC_RESPOND_POOL_HPP__

#include <cstdint>
#include <net/base/map.hpp>
#include <net/rpc/datagram.hpp>


namespace pan { namespace net { namespace rpc {

using respond_pool = pan::map<std::uint16_t, datagram_ptr>;

}}}

#endif // __PAN_NET_RPC_RESPOND_POOL_HPP__