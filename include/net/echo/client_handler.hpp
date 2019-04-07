#ifndef __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
#define __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__

#include <string>
#include <iostream>
#include <net/handler_base.hpp>

namespace pan { namespace net { namespace echo {

template <typename Session>
class client_handler
    : public pan::net::handler_base<Session> {
public:
    template <typename Message>
    void write(const Message& message)
    {
        if (session_) {
            session_->write(message.data(), message.size());
        }
    }

protected:
    void on_start(session_ptr session) 
    {
        session_ = session;
    }

    std::size_t on_message(session_ptr /*session*/, const void* buffer, std::size_t size)
    {
        std::cout << std::string((char*)buffer, (char*)buffer + size) << std::endl;
        return size;
    }

private:
    session_ptr session_;

};

}}}

#endif // __PAN_NET_TCP_ECHO_CLIENT_HANDLER_HPP__
