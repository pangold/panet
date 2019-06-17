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
        if (!session_) {
            LOG_ERROR("echo.client.handler.send: session is not ready yet.");
            return;
        }
        session_->write(data.data(), data.size());
    }

protected:
    void on_session_start(session_ptr session) override
    {
        session_ = session;
    }

    void on_session_stop(session_ptr) override
    {
        session_.reset();
    }

    size_t on_message(session_ptr, const void* data, size_t size) override
    {
        std::string msg((char*)data, (char*)data + size);
        LOG_INFO("echo client: recevied: %s", msg.c_str());
        return size;
    }

private:
    session_ptr session_;

};

}}}

#endif // __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
