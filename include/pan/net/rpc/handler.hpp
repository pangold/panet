#ifndef __PAN_NET_RPC_HANDLER_HPP__
#define __PAN_NET_RPC_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/rpc/rpc.pb.h>
#include <pan/net/rpc/requester.hpp>
#include <pan/net/rpc/responder.hpp>

namespace pan { namespace net { namespace rpc {

class handler : public protocol::datagram_handler_base<handler> {
    typedef protobuf::codec<session_type> codec_type;
    typedef processor<session_type, Pango::Rpc::Request> requester_type;
    typedef processor<session_type, Pango::Rpc::Respond> responder_type;
public:
    handler() 
        : codec_()
        , requester_(pool_, codec_)
        , responder_(pool_, codec_)
    {
        
    }
    template <typename F, typename... Args>
    void bind(const std::string& name, F func, Args... args)
    {
        requester_.bind(name, func, std::forward<Args>(args)...);
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
    void on_datagram(session_ptr session, datagram_ptr datagram)
    {
        // protobuf::codec has responsibilities to dispatch
        codec_.on_message(session, *datagram);
    }
protected:
    codec_type codec_;
    requester_type requester_;
    responder_type responder_;

};

}}}

#endif // __PAN_NET_RPC_HANDLER_HPP__
