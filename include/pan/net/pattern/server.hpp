#ifndef __PAN_NET_PATTERN_SERVER_HPP__
#define __PAN_NET_PATTERN_SERVER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/pattern/handler.hpp>

namespace pan { namespace net { namespace pattern {

class server : public tcp::server<handler> {
public:
    explicit server(uint16_t port)
        : tcp::server<handler>(port)
    {

    }
};

}}}

#endif // __PAN_NET_PATTERN_SERVER_HPP__