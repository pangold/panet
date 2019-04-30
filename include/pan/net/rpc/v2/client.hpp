#ifndef __PAN_NET_RPC_CLIENT_V2_HPP__
#define __PAN_NET_RPC_CLIENT_V2_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/v2/client_handler.hpp>

namespace pan { namespace net { namespace rpc { namespace v2 {

class client : tcp::basic_client<client_handler<tcp::session> > {
public:
    client(const std::string& host, std::uint16_t port)
        : tcp::basic_client<client_handler<tcp::session> >(host, port)
    { }
    
    void set_timeout(std::size_t timeout)
    {
        handler().set_timeout(timeout);
    }
    
    template <typename R, typename... Args>
    R call(const std::string& name, Args... args)
    {
        R result;
        handler().call(result, name, std::forward<Args>(args)...);
        return result;
    }

    template <typename R, typename... Args>
    bool call(R& result, const std::string& name, Args... args)
    {
        return handler().call(result, name, std::forward<Args>(args)...);
    }

    std::string last_error() const
    {
        return std::string("coming soon");
    }
    
};

}}}}

#endif // __PAN_NET_RPC_CLIENT_V2_HPP__