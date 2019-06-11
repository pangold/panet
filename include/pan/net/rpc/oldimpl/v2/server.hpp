#ifndef __PAN_NET_RPC_SERVER_V2_HPP__
#define __PAN_NET_RPC_SERVER_V2_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/rpc/v2/server_handler.hpp>

namespace pan { namespace net { namespace rpc { namespace v2 {

class server : tcp::basic_server<server_handler<tcp::session> > {
public:
    server(std::uint16_t port)
        : tcp::basic_server<server_handler<tcp::session> >(port)
    { }
    
    void run()
    {
        tcp::basic_server<server_handler<tcp::session> >::run();
    }
    
    template <typename F>
    void bind(const std::string& name, F func)
    {
        handler().bind(name, func);
    }
    
    template <typename F, typename O>
    void bind(const std::string& name, F func, O* obj)
    {
        handler().bind(name, func, obj);
    }
};

}}}}

#endif // __PAN_NET_RPC_SERVER_V2_HPP__