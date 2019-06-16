#ifndef __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__
#define __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__

#include <string>
#include <pan/net/tcp.hpp>

namespace pan { namespace net { namespace echo {

class server_handler : public tcp::handler_base<server_handler> {
    friend class session_type;
public:
    explicit server_handler(session_pool_type& pool)
        : tcp::handler_base<server_handler>(pool)
    { }

protected:
    size_t on_message(session_ptr session, const void* data, size_t size) override
    {
        session->write(data, size);
        return size;
    }
};

}}}

#endif // __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__
