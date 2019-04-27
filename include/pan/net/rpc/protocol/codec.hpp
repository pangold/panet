#ifndef __PAN_NET_RPC_PROTOCOL_CODEC_HPP__
#define __PAN_NET_RPC_PROTOCOL_CODEC_HPP__

#include <pan/base.hpp>
#include <pan/net/rpc/protocol/datagram.hpp>
#include <pan/net/rpc/protocol/utils.hpp>

namespace pan { namespace net { namespace rpc {

template <typename Session>
class codec {
    using session_type = Session;
    using session_ptr = typename session_type::pointer;
    using callback_type = std::function<void(session_ptr, datagram_ptr)>;

public:
    void register_callback(callback_type cb)
    {
        callback_ = cb;
    }

    void send(session_ptr session, datagram_ptr datagram)
    {
        bit::ostream os;
        os << *datagram; // serialize
        session->write(os.data(), os.size());
    }

    std::size_t on_message(session_ptr session, const void* data, std::size_t size)
    {
        bit::istream is((char*)data, size);
        while (true) {
            if (is.size() == is.capacity() || is.remaining_size() < RPC_HEADER_LEN) {
                break;
            }
            auto datagram = make_datagram();
            is >> *datagram;
            if (!confirm(*datagram)) {
                break;
            }
            callback_(session, datagram);
        }
        return is.size();
    }

private:
    callback_type callback_;

};

}}}

#endif // __PAN_NET_RPC_PROTOCOL_CODEC_HPP__