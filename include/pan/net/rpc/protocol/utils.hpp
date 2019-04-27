#ifndef __PAN_NET_RPC_PROTOCOL_UTILS_HPP__
#define __PAN_NET_RPC_PROTOCOL_UTILS_HPP__

#include <pan/base.hpp>
#include <pan/net/rpc/protocol/datagram.hpp>

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

inline static datagram_ptr make_datagram()
{
    return std::make_shared<struct datagram>();
}

inline static datagram_ptr make_reply(std::uint32_t message_id, std::uint8_t code)
{
    datagram_ptr datagram = make_datagram();
    datagram->message_id = message_id;
    datagram->status_code = code;
    datagram->request = RPC_RESPOND;
    datagram->body_size = 0;
    return datagram;
}

inline static datagram_ptr make_reply(std::uint32_t message_id, std::uint8_t code, const void* data, std::uint32_t size)
{
    datagram_ptr datagram = make_datagram();
    datagram->message_id = message_id;
    datagram->status_code = code;
    datagram->request = RPC_RESPOND;
    datagram->body_size = size;
    datagram->body.assign((char*)data, (char*)data + size);
    return datagram;
}

inline static datagram_ptr make_request(std::uint32_t message_id)
{
    datagram_ptr datagram = make_datagram();
    datagram->message_id = message_id;
    datagram->request = RPC_REQUEST;
    datagram->body_size = 0;
    return datagram;
}

inline static datagram_ptr make_request(std::uint32_t message_id, const void* data, std::uint32_t size)
{
    datagram_ptr datagram = make_datagram();
    datagram->message_id = message_id;
    datagram->request = RPC_REQUEST;
    datagram->body_size = size;
    datagram->body.assign((char*)data, (char*)data + size);
    return datagram;
}

inline static datagram_ptr make_heart_beat()
{
    datagram_ptr datagram = make_datagram();
    datagram->heart_beat = RPC_HEART_BEAT_ON;
    return datagram;
}

inline static bool confirm(const struct datagram& datagram)
{
    switch (datagram.version) {
    case RPC_DATAGRAM_V1:
        return datagram.magic == RPC_MAGIC &&
            datagram.header_size == RPC_HEADER_LEN &&
            datagram.body.size() == datagram.body_size;
    }
    return false;
}

}}}

#endif // __PAN_NET_RPC_PROTOCOL_UTILS_HPP__