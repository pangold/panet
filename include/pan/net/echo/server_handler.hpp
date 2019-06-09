#ifndef __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__
#define __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__

#include <pan/net/tcp.hpp>
#include <pan/net/tcp/handler_base.hpp>

namespace pan { namespace net { namespace echo {

class server_handler : public tcp::handler_base<server_handler> {
public:
    std::size_t on_message(session_ptr session, const void* data, std::size_t size)
    {
        session->write(data, size);
        return size;
    }
};

}}}

#endif // __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__
