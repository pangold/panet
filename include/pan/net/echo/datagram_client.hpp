#ifndef __PAN_NET_ECHO_DATAGRAM_CLIENT_HPP__
#define __PAN_NET_ECHO_DATAGRAM_CLIENT_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/echo/datagram_client_handler.hpp>

namespace pan { namespace net { namespace echo {

class datagram_client : public tcp::client<datagram_client_handler> {
    typedef tcp::client<datagram_client_handler> _Mybase;
public:
    datagram_client(const std::string& host, uint16_t port)
        : _Mybase(host, port)
    { }

    void write_datagram(const std::string& name, const std::string& data)
    {
        protocol::datagram datagram(name, data);
        std::string str;
        protocol::datagram_to_data(str, datagram);
        write(str.data(), str.size());
    }
};

}}}

#endif // __PAN_NET_ECHO_DATAGRAM_CLIENT_HPP__