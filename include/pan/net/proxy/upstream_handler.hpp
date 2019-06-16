#ifndef __PAN_NET_PROXY_UPSTREAM_HANDLER_HPP__
#define __PAN_NET_PROXY_UPSTREAM_HANDLER_HPP__

#include <functional>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace proxy {

class upstream_handler : public protocol::datagram_handler_base<upstream_handler> {
public:
    typedef std::function<void(session_ptr, datagram_ptr)> message_callback_type;

    explicit upstream_handler(session_pool_type& pool)
        : protocol::datagram_handler_base<upstream_handler>(pool)
    {

    }

    void register_message_callback(message_callback_type cb)
    {
        message_callback_ = std::move(cb);
    }

    bool write(const std::string& service_name, const void* data, size_t size)
    {
        auto session = pool()[service_name];
        if (session) {
            session->write(data, size);
        }
        return session != nullptr;
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

#endif // __PAN_NET_PROXY_UPSTREAM_HANDLER_HPP__