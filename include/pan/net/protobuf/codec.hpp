#ifndef __PAN_NET_PROTOBUF_CODEC_HPP__
#define __PAN_NET_PROTOBUF_CODEC_HPP__

#include <memory>
#include <google/protobuf/message.h>
#include <pan/base.hpp>
#include <pan/net/protobuf/datagram.hpp>
#include <pan/net/protobuf/utils.hpp>
#include <pan/net/protobuf/proxy.hpp>

namespace pan { namespace net { namespace protobuf {

template <typename Session>
class codec {
    using session_type = Session;
    using session_ptr = typename session_type::pointer;
    using proxy_type = proxy<session_type>;

public:
    template <typename T>
    void register_callback(std::function<void(session_ptr, std::shared_ptr<T>)> cb)
    {
        proxy_.register_callback(cb);
    }

    void send(session_ptr session, message_ptr message)
    {
        ostream os;
        os << *serialize_from_message(message);
        if (session) {
            session->write(os.data(), os.size());
        }
    }

    std::size_t on_message(session_ptr session, const void* data, std::size_t size)
    {
        istream is(data, size);
        auto datagram = parse_to_datagram(is);
        if (datagram && auto message = parse_to_message(datagram)) {
            proxy_.on_message(session, message);
            return is.size();
        }
        return datagram ? size : 0;
    }

private:
    proxy_type proxy_;

};

}}}

#endif // __PAN_NET_PROTOBUF_CODEC_HPP__