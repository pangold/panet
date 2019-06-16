#ifndef __PAN_NET_RPC_SERVER_HANDLER_HPP__
#define __PAN_NET_RPC_SERVER_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf.hpp>
#include <pan/net/rpc/rpc.pb.h>
#include <pan/net/rpc/requester.hpp>

namespace pan { namespace net { namespace rpc {

class server_handler : public protocol::datagram_handler_base<server_handler> {
    friend class session_type;
    typedef protobuf::codec<session_type> codec_type;
    typedef requester<session_type> requester_type;
public:
    explicit server_handler(session_pool_type& pool)
        : protocol::datagram_handler_base<server_handler>(pool)
        , codec_()
        , requester_(codec_)
    {
        
    }
    template <typename F, typename... Args>
    void bind(const std::string& name, F func, Args... args)
    {
        requester_.bind(name, func, std::forward<Args>(args)...);
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

};

}}}

#endif // __PAN_NET_RPC_SERVER_HANDLER_HPP__
