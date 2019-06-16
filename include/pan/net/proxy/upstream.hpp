#ifndef __PAN_NET_PROXY_UPSTREAM_HPP__
#define __PAN_NET_PROXY_UPSTREAM_HPP__

#include <string>
#include <iostream>
#include <pan/net/proxy/upstream_handler.hpp>
#include <pan/net/tcp.hpp>

namespace pan { namespace net { namespace proxy {

class upstream : public tcp::clients<proxy::upstream_handler> {
public:
    upstream(const std::string& host, uint16_t port)
        : tcp::clients<proxy::upstream_handler>(host, port)
    { }

    template <typename Callback>
    void register_message_callback(Callback cb)
    {
        handler_.register_message_callback(cb);
    }

    bool write(const std::string& name, const std::string& data)
    {
        return handler_.write(name, data.data(), data.size());
    }
};

}}}

#endif // __PAN_NET_PROXY_UPSTREAM_HPP__
