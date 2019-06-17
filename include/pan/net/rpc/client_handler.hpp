#ifndef __PAN_NET_RPC_CLIENT_HANDLER_HPP__
#define __PAN_NET_RPC_CLIENT_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/protocol.hpp>
#include <pan/net/rpc/rpc.pb.h>
#include <pan/net/rpc/responder.hpp>

namespace pan { namespace net { namespace rpc {

class client_handler : public tcp::handler_base<client_handler> {
    friend class session_type;
    typedef protocol::codec<session_type> codec_type;
    typedef responder<session_type> responder_type;
public:
    explicit client_handler(session_pool_type& pool)
        : tcp::handler_base<client_handler>(pool)
        , codec_()
        , responder_(codec_)
    {
        
    }

    void set_timeout(size_t timeout)
    {
        responder_.set_timeout(timeout);
    }

    template <typename R, typename... Args>
    R call(std::string& error, const std::string& name, Args... args)
    {
        return responder_.call<R>(error, name, std::forward<Args>(args)...);
    }

protected:
    void on_session_start(session_ptr session) override
    {
        responder_.set_session(session);
    }

    void on_session_stop(session_ptr session) override
    {
        responder_.set_session(nullptr);
    }

    size_t on_message(session_ptr session, const void* data, size_t size) override
    {
        // protocol::codec has responsibilities to dispatch
        return codec_.on_message(session, data, size);
    }

protected:
    codec_type codec_;
    responder_type responder_;

};

}}}

#endif // __PAN_NET_RPC_CLIENT_HANDLER_HPP__
