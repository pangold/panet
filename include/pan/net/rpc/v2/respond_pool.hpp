#ifndef __PAN_NET_RPC_RESPOND_POOL_V2_HPP__
#define __PAN_NET_RPC_RESPOND_POOL_V2_HPP__

#include <cstdint>
#include <pan/base.hpp>
#include "rpc.pb.h"

namespace pan { namespace net { namespace rpc { namespace v2 {

using respond_pool = pan::map<std::int32_t, std::shared_ptr<Pango::Rpc::Respond> >;

}}}}

#endif // __PAN_NET_RPC_RESPOND_POOL_V2_HPP__