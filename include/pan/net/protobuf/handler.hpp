#ifndef __PAN_NET_PROTOBUF_HANDLER_HPP__
#define __PAN_NET_PROTOBUF_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/tcp/handler_base.hpp>
#include <pan/net/protobuf/codec.hpp>

namespace pan { namespace net { namespace protobuf {

// Now, the problem is how to combine protobuf_handler_base with message_base/proxy_handler_base
// should be another typename Message that inherit message_base
// concrete_message<ProtobufMessage>::on_message<PtobufMessage>(session_ptr, ProtobufMessage)
// register this on_message above to codec_
// at last, use a vector to hold these messages
template <typename Inherit>
class handler_base : public tcp::handler_base<Inherit> {
public:
    template <typename T>
    using callback_type = std::function<void(session_ptr, std::shared_ptr<T>)>;

    handler_base()
    {
        codec_.register_callback<T>(std:bind)
    }
    
    // should 
    template <typename T>
    void register_message_callback(callback_type<T> cb)
    {
        codec_.register_callback<T>(std::move(cb));
    }

    template <typename T>
    void on_protobuf_message(session_ptr session, std::shared_ptr<T> message)
    {

    }
    
    std::size_t on_message(session_ptr session, const void* data, std::size_t size) 
    { 
        auto use_len = codec_.on_message(session, data, size);
        if (use_len < 0) {
            LOG_FATAL("protobuf.on.message: data error, stop session %s", session->to_string().c_str());
            session->stop();
        }
        return static_cast<std::size_t>(use_len < 0 ? 0 : use_len);
    }
    
private:
    protobuf::codec<session_type> codec_;

};

}}}

#endif // __PAN_NET_PROTOBUF_HANDLER_HPP__
