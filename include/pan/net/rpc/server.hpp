#ifndef __PAN_NET_RPC_SERVER_HPP__
#define __PAN_NET_RPC_SERVER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/handler.hpp>

namespace pan { namespace net { namespace rpc {

class server : public tcp::server<rpc::handler> {
public:
    server(uint16_t port)
        : tcp::server<handler>(port)
    {

    }
    template <typename F, typename... Args>
    void bind(const std::string& name, F func, Args... args)
    {
        handler_.bind(name, func, std::forward<Args>(args)...);
    }
};

}}}

#endif // __PAN_NET_RPC_SERVER_HPP__