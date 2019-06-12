#ifndef __PAN_NET_PUBSUB_SERVER_HPP__
#define __PAN_NET_PUBSUB_SERVER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/pub_sub/handler.hpp>

namespace pan { namespace net { namespace pubsub {

template <typename Storage>
class server : public tcp::server<pubsub::handler<Storage>> {
public:
    server(uint16_t port, bool use_thread = false)
        : tcp::server<handler<Storage>>(port, use_thread)
    {

    }

};

}}}

#endif // __PAN_NET_PUBSUB_SERVER_HPP__