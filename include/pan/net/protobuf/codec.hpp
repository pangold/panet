#ifndef __PAN_NET_PROTOBUF_CODEC_HPP__
#define __PAN_NET_PROTOBUF_CODEC_HPP__

#include <memory>
#include <pan/base.hpp>
#include <pan/net/protocol.hpp>
#include <pan/net/protobuf/dispatcher.hpp>
#include <pan/net/protobuf/helper.hpp>

namespace pan { namespace net { namespace protobuf {

template <typename Session>
class codec {
    typedef Session session_type;
    typedef typename session_type::pointer session_ptr;
    typedef dispatcher<session_type> dispatcher_type;
public:
    template <typename T>
    void register_callback(std::function<void(session_ptr, std::shared_ptr<T>)> cb)
    {
        dispatcher_.register_callback(cb);
    }

    bool send(session_ptr session, message_ptr message)
    {
        std::string str;
        if (serialize(str, message)) {
            session->write(str.data(), str.size());
            return true;
        }
        return false;
    }

    bool on_message(session_ptr session, const protocol::datagram& datagram)
    {
        message_ptr message;
        if (datagram_to_message(datagram, message)) {
            dispatcher_.on_message(session, message);
            return true;
        }
        return false;
    }

    bool on_message(session_ptr session, const void* data, size_t size)
    {
        message_ptr message;
        if (parse(message, data, size) > 0) {
            dispatcher_.on_message(session, message);
            return true;
        }
        return false;
    }

private:
    dispatcher_type dispatcher_;

};

}}}

#endif // __PAN_NET_PROTOBUF_CODEC_HPP__