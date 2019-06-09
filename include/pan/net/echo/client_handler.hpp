#ifndef __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
#define __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__

#include <string>
#include <iostream>
#include <pan/base.hpp>
#include <pan/net/tcp.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace echo {

class client_handler : public tcp::handler_base<client_handler> {
public:
    std::size_t on_message(session_ptr, const void* data, std::size_t size)
    {
        std::string msg((char*)data, (char*)data + size);
        LOG_INFO("echo client: recevied: %s", msg.c_str());
        return size;
    }
};

}}}

#endif // __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
