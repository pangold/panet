#ifndef __PAN_NET_PROTOBUF_CODEC_HPP__
#define __PAN_NET_PROTOBUF_CODEC_HPP__

#include <memory>
#include <pan/base.hpp>
#include <pan/net/protocol/datagram.hpp>
#include <pan/net/protocol/utils.hpp>
#include <pan/net/protobuf/dispatcher.hpp>
#include <pan/net/protobuf/helper.hpp>

namespace pan { namespace net { namespace protobuf {

template <typename Session>
class codec {
    using session_type = Session;
    using session_ptr = typename session_type::pointer;
    using dispatcher_type = dispatcher<session_type>;

public:
    template <typename T>
    void register_callback(std::function<void(session_ptr, std::shared_ptr<T>)> cb)
    {
        dispatcher_.register_callback(cb);
    }

    void send(session_ptr session, message_ptr message)
    {
        ostream os;
        os << *serialize_from_message(message);
        if (session) {
            session->write(os.data(), os.size());
        }
    }

    int on_message(session_ptr session, const void* data, std::size_t size)
    {
        istream is(data, size);
        auto d = std::make_shared<datagram>();
        auto len = parse_to_datagram(is, d);
        if (len <= 0) return len;
        if (auto message = parse_to_message(d)) {
            dispatcher_.on_message(session, message);
            return len;
        }
        return static_cast<int>(size);
    }

private:
    dispatcher_type dispatcher_;

};

}}}

#endif // __PAN_NET_PROTOBUF_CODEC_HPP__