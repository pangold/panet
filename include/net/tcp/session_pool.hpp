#ifndef __PAN_NET_TCP_POOL_HPP__
#define __PAN_NET_TCP_POOL_HPP__

#include <net/base/map.hpp>
#include <net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {
    
using session_pool = pan::map<session::key_type, session::pointer>;

}}}

#endif // __PAN_NET_TCP_POOL_HPP__
