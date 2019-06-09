#ifndef __PAN_NET_PROXY_UPSTREAM_HPP__
#define __PAN_NET_PROXY_UPSTREAM_HPP__

#include <functional>
#include <pan/net/protocol/datagram_handler_base.hpp>

namespace pan { namespace net { namespace proxy {

class upstream : public protocol::datagram_handler_base<upstream> {
public:
    typedef std::function<void(session_ptr, datagram_ptr)> message_callback_type;
    void register_message_callback(message_callback_type cb)
    {
        message_callback_ = std::move(cb);
    }
private:
    void on_datagram(session_ptr session, datagram_ptr datagram)
    {
        if (message_callback_)
            message_callback_(session, datagram);
    }
private:
    message_callback_type message_callback_;
};

}}}

#endif // __PAN_NET_PROXY_UPSTREAM_HPP__