#ifndef __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
#define __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__

#include <string>
#include <iostream>
#include <pan/net/tcp.hpp>

namespace pan { namespace net { namespace echo {

class client_handler : public tcp::handler_base<client_handler> {
    friend class session_type;
public:
    explicit client_handler(session_pool_type& pool)
        : tcp::handler_base<client_handler>(pool)
    { }

    void write(const std::string& data)
    {
        pool().front()->write(data.data(), data.size());
    }

protected:
    size_t on_message(session_ptr, const void* data, size_t size) override
    {
        std::string msg((char*)data, (char*)data + size);
        LOG_INFO("echo client: recevied: %s", msg.c_str());
        return size;
    }
};

}}}

#endif // __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
