#ifndef __PAN_NET_ECHO_CLIENT_HPP__
#define __PAN_NET_ECHO_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/client_handler.hpp>

namespace pan { namespace net { namespace echo {

class client : public tcp::client<echo::client_handler> {
    typedef tcp::client<echo::client_handler> _Mybase;
public:
    client(const std::string& host, uint16_t port)
        : _Mybase(host, port)
    { }

    void write(const std::string& data)
    {
        pool_.wait();
        handler_.write(data);
    }
};

}}}

#endif // __PAN_NET_ECHO_CLIENT_HPP__