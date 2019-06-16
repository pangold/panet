#ifndef __PAN_NET_ECHO_DATAGRAM_CLIENT_HANDLER_HPP__
#define __PAN_NET_ECHO_DATAGRAM_CLIENT_HANDLER_HPP__

#include <pan/base.hpp>
#include <pan/net/tcp.hpp>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace echo {

class datagram_client_handler : public protocol::datagram_handler_base<datagram_client_handler> {
    friend class session_type;
public:
    explicit datagram_client_handler(session_pool_type& pool)
        : protocol::datagram_handler_base<datagram_client_handler>(pool)
    {

    }

    void write_datagram(const std::string& name, const std::string& data)
    {
        protocol::datagram datagram(name, data);
        std::string str;
        protocol::datagram_to_data(str, datagram);
        pool().front()->write(str.data(), str.size());
    }

protected:
    void on_datagram(session_ptr session, std::shared_ptr<pan::net::protocol::datagram> datagram)
    {
        LOG_INFO("received from %s: %s", session->to_string().c_str(), datagram->data().c_str());
    }
    void on_write(session_ptr session, const void* data, std::size_t size)
    {
        pan::net::protocol::datagram datagram(data, size);
        LOG_INFO("sent to %s: %s", session->to_string().c_str(), datagram.data().c_str());
    }
};

}}}

#endif // __PAN_NET_ECHO_DATAGRAM_CLIENT_HANDLER_HPP__