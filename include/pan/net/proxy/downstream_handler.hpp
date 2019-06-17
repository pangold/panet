#ifndef __PAN_NET_PROXY_DOWNSTREAM_HANDLER_HPP__
#define __PAN_NET_PROXY_DOWNSTREAM_HANDLER_HPP__

#include <pan/base.hpp>
#include <pan/net/tcp.hpp>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace proxy {

class downstream_handler : public tcp::handler_base<downstream_handler> {
    friend class session_type;
public:
    typedef protocol::codec<session_type> codec_type;
    typedef codec_type::datagram_ptr datagram_ptr;

    explicit downstream_handler(session_pool_type& pool)
        : tcp::handler_base<downstream_handler>(pool)
        , codec_()
    {
        using namespace std::placeholders;
        codec_.register_datagram_callback(std::bind(&downstream_handler::on_datagram, this, _1, _2));
    }

    void write(const std::string& name, const std::string& data)
    {
        protocol::datagram datagram(name, data);
        if (!session_) {
            LOG_ERROR("datagram.client.handler.send: session is not ready yet.");
            return;
        }
        codec_.send(session_, datagram);
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
    size_t on_message(session_ptr session, const void* data, size_t size) override
    {
        return codec_.on_message(session, data, size);
    }
    void on_write(session_ptr session, const void* data, std::size_t size) override
    {
        protocol::datagram datagram(data, size);
        LOG_INFO("sent to %s: %s", session->to_string().c_str(), datagram.data().c_str());
    }
    void on_datagram(session_ptr session, datagram_ptr datagram)
    {
        LOG_INFO("received from %s: %s", session->to_string().c_str(), datagram->data().c_str());
    }

protected:
    codec_type codec_;
    session_ptr session_;

};

}}}

#endif // __PAN_NET_PROXY_DOWNSTREAM_HANDLER_HPP__