#ifndef __PAN_NET_PROTOCOL_CODEC_HPP__
#define __PAN_NET_PROTOCOL_CODEC_HPP__

#include <memory>
#include <pan/base.hpp>
#include <pan/net/protocol/datagram.hpp>
#include <pan/net/protocol/dispatcher.hpp>

namespace pan { namespace net { namespace protocol {
    
template <typename Session, 
    typename Datagram = datagram>
class codec {
public:
    typedef Session session_type;
    typedef Datagram datagram_type;
    typedef protobuf::dispatcher<session_type> dispatcher_type;
    typedef typename session_type::pointer session_ptr;
    typedef typename datagram_type::pointer datagram_ptr;
    typedef std::function<void(session_ptr, datagram_ptr)> datagram_callback_type;

    template <typename T>
    void register_callback(std::function<void(session_ptr, std::shared_ptr<T>)> cb)
    {
        dispatcher_.register_callback(std::move(cb));
    }

    void register_datagram_callback(datagram_callback_type cb)
    {
        datagram_callback_ = std::move(cb);
    }

    size_t send(session_ptr session, const datagram_type& datagram)
    {
        std::string str;
        if (!datagram.to(str)) {
            LOG_ERROR("protocol.datagram.serialize.error: %s", datagram.name().c_str());
            return 0;
        }
        session->write(str.data(), str.size());
        return str.size();
    }

    size_t send(session_ptr session, datagram_ptr datagram)
    {
        return send(session, *datagram);
    }

    size_t send(session_ptr session, protobuf::message_ptr message)
    {
        datagram_type datagram;
        datagram.set_id(std::chrono::steady_clock::now().time_since_epoch().count() / 1000);
        if (!datagram.from(message)) {
            LOG_ERROR("protocol.serialize.error: %s", message->DebugString().c_str());
            return 0;
        }
        return send(session, datagram);
    }

    size_t on_message(session_ptr session, const void* data, size_t size)
    {
        size_t processed_size = 0;
        while (true) {
            const char* begin = (const char*)data + processed_size;
            size_t remaining = size - processed_size;
            auto datagram = std::make_shared<datagram_type>(begin, remaining);
            if (!datagram->completed()) break;
            if (!datagram->valid()) {
                LOG_ERROR("protocol.datagram.parse.error: invalid checksum");
                return size;
            }
            if (datagram_callback_) datagram_callback_(session, datagram);
            else on_datagram(session, datagram);
            processed_size += datagram->size();
        }
        return processed_size;
    }

    size_t on_datagram(session_ptr session, datagram_ptr datagram)
    {
        protobuf::message_ptr message;
        if (!datagram->to(message) || !message) {
            LOG_ERROR("protocol.parse.error: name %s", datagram->name().c_str());
            return 0;
        }
        dispatcher_.on_message(session, message);
        return datagram->size();
    }

private:
    datagram_callback_type datagram_callback_;
    dispatcher_type dispatcher_;

};

}}}

#endif // __PAN_NET_PROTOCOL_CODEC_HPP__