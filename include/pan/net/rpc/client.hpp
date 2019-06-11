#ifndef __PAN_NET_RPC_CLIENT_HPP__
#define __PAN_NET_RPC_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/handler.hpp>

namespace pan { namespace net { namespace rpc {

class client : private tcp::client<rpc::handler> {
public:
    client(const std::string& host, uint16_t port, size_t timeout = 3000)
        : tcp::client<handler>(host, port)
    {
        handler_.set_timeout(timeout);
    }
    template <typename R, typename... Args>
    R call(const std::string& name, Args... args)
    {
        std::string error;
        wait_for_session();
        return handler_.call<R, Args...>(error, name, std::forward<Args>(args)...);
    }
    template <typename R, typename... Args>
    R call2(std::string& error, const std::string& name, Args... args)
    {
        wait_for_session();
        return handler_.call<R, Args...>(error, name, std::forward<Args>(args)...);
    }
};

}}}

#endif // __PAN_NET_RPC_CLIENT_HPP__