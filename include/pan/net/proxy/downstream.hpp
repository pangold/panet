#ifndef __PAN_NET_PROXY_DOWNSTREAM_HPP__
#define __PAN_NET_PROXY_DOWNSTREAM_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/proxy/downstream_handler.hpp>

namespace pan { namespace net { namespace proxy {

class downstream : public tcp::client<downstream_handler> {
public:
    downstream(const std::string& host, uint16_t port)
        : tcp::client<downstream_handler>(host, port)
    { }

    void write(const std::string& name, const std::string& data)
    {
        pool_.wait();
        handler_.write(name, data);
    }
};

}}}

#endif // __PAN_NET_PROXY_DOWNSTREAM_HPP__