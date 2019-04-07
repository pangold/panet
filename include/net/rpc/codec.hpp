#ifndef __PAN_NET_RPC_CODEC_HPP__
#define __PAN_NET_RPC_CODEC_HPP__

#include <net/base/stream.hpp>
#include <net/rpc/datagram.hpp>

namespace pan { namespace net { namespace rpc {

inline static bit::ostream& operator << (bit::ostream& os, const datagram& datagram)
{
    os << datagram.magic << datagram.header_size;
    os << datagram.version;
    os.write_bits(datagram.stype, 5);
    os.write_bits(datagram.heart_beat, 1);
    os.write_bits(datagram.one_way, 1);
    os.write_bits(datagram.request, 1);
    os << datagram.status_code << datagram.reserved;
    os << datagram.message_id << datagram.body_size;
    if (datagram.body_size > 0) {
        os.write_bytes(datagram.body.data(), datagram.body_size);
    }
    return os;
}

inline static bit::istream& operator >> (bit::istream& is, datagram& datagram)
{
    is >> datagram.magic >> datagram.header_size;
    is >> datagram.version;
    if (datagram.magic != RPC_MAGIC || 
        datagram.header_size != RPC_HEADER_LEN || 
        is.capacity() < RPC_HEADER_LEN) {
        // not a complete datagram
        return is;
    }
    is.read_bits(datagram.stype, 5);
    is.read_bits(datagram.heart_beat, 1);
    is.read_bits(datagram.one_way, 1);
    is.read_bits(datagram.request, 1);
    is >> datagram.status_code >> datagram.reserved;
    is >> datagram.message_id >> datagram.body_size;
    if (is.remaining_size() < datagram.body_size) {
        // not a complete datagram
        return is;
    }
    std::vector<char> data(datagram.body_size);
    is.read_bytes(data.data(), data.size());
    std::swap(datagram.body, data);
    return is;
}

}}}

#endif // __PAN_NET_RPC_CODEC_HPP__