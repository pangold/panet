#ifndef __PAN_NET_ECHO_DATAGRAM_CLIENT_HPP__
#define __PAN_NET_ECHO_DATAGRAM_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/datagram_client_handler.hpp>

namespace pan { namespace net { namespace echo {

class datagram_client : public tcp::client<datagram_client_handler> {
public:
    datagram_client(const std::string& host, uint16_t port)
        : tcp::client<datagram_client_handler>(host, port)
    { }

    void write_datagram(const std::string& name, const std::string& data)
    {
        pool_.wait();
        handler_.write_datagram(name, data);
    }
};

}}}

#endif // __PAN_NET_ECHO_DATAGRAM_CLIENT_HPP__