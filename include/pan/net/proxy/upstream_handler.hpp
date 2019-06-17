#ifndef __PAN_NET_PROXY_UPSTREAM_HANDLER_HPP__
#define __PAN_NET_PROXY_UPSTREAM_HANDLER_HPP__

#include <functional>
#include <pan/net/tcp.hpp>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace proxy {

class upstream_handler : public tcp::handler_base<upstream_handler> {
    friend class session_type;
public:
    typedef protocol::codec<session_type> codec_type;
    typedef codec_type::datagram_ptr datagram_ptr;

    explicit upstream_handler(session_pool_type& pool)
        : tcp::handler_base<upstream_handler>(pool)
    {

    }

    template <typename Callback>
    void register_datagram_callback(Callback cb)
    {
        codec_.register_datagram_callback(std::move(cb));
    }

    bool write(const std::string& service_name, const void* data, size_t size)
    {
        if (auto session = pool()[service_name]) {
            session->write(data, size);
            return true;
        }
        return false;
    }

private:
    void on_session_start(session_ptr session) override
    {

    }

    void on_session_stop(session_ptr) override
    {

    }

    size_t on_message(session_ptr, const void* data, size_t size) override
    {
        return size;
    }

private:
    codec_type codec_;

};

}}}

#endif // __PAN_NET_PROXY_UPSTREAM_HANDLER_HPP__