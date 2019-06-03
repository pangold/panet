#ifndef __PAN_NET_PROTOBUF_HANDLER_HPP__
#define __PAN_NET_PROTOBUF_HANDLER_HPP__

#include <memory>
#include <functional>
#include <pan/net/tcp/handler_base.hpp>
#include <pan/net/protobuf/codec.hpp>

namespace pan { namespace net { namespace protobuf {

class handler : public pan::net::tcp::handler_base {
public:
    template <typename T>
    using callback_type = std::function<void(session_ptr, std::shared_ptr<T>)>;
    
    template <typename T>
    void register_message_callback(callback_type<T> cb)
    {
        codec_.register_callback<T>(std::move(cb));
    }
    
    std::size_t on_message(session_ptr session, const void* data, std::size_t size) 
    { 
        try {
            return codec_.on_message(session, data, size);
        }
        catch (const protobuf::data_error& e) {
            session->stop();
            LOG_FATAL(e.what());
        }
        catch (std::exception& e) {
            LOG_ERROR(e.what());
        }
        return 0;
    }
    
private:
    protobuf::codec<session_type> codec_;

};

}}}

#endif // __PAN_NET_PROTOBUF_HANDLER_HPP__
