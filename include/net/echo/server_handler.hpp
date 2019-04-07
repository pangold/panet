#ifndef __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__
#define __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__

#include <net/handler_base.hpp>

namespace pan { namespace net { namespace echo {

template <typename Session>
class server_handler
    : public pan::net::handler_base<Session> {
protected:
    std::size_t on_message(session_ptr session, const void* buffer, std::size_t size)
    {
        session->write(buffer, size);
        return size;
    }
};

}}}

#endif // __PAN_NET_TCP_ECHO_SERVER_HANDLER_HPP__
