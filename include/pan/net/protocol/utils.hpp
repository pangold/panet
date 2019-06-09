#ifndef __PAN_NET_PROTOCOL_UTILS_HPP__
#define __PAN_NET_PROTOCOL_UTILS_HPP__

#include <memory>
#include <pan/net/protocol/datagram.hpp>

namespace pan { namespace net { namespace protocol {

inline static int data_to_datagram(datagram& datagram, const void* data, size_t size)
{
    if (!datagram.from(data, size)) {
        return 0;
    }
    if (!datagram.valid()) {
        LOG_ERROR("protocol.datagram.parse: invalid checksum");
        return -1;
    }
    return datagram.size();
}

inline static bool datagram_to_data(std::string& str, const datagram& datagram)
{
    if (!datagram.to(str)) {
        LOG_ERROR("protocol.datagram.serialize: serialize %s error", datagram.name().c_str());
        return false;
    }
    return true;
}

template <typename Callback>
static int codec_datagrams(const void* data, size_t size, Callback cb)
{
    int process_size = 0;
    while (true) {
        const char* begin = (const char*)data + process_size;
        int remaining_size = size - process_size;
        // not enough space for a datagram, even though datagram::length
        if (remaining_size < 4) break;
        auto datagram = std::make_shared<protocol::datagram>();
        int len = data_to_datagram(*datagram, begin, remaining_size);
        if (len == -1) return -1;
        else if (len == 0) break;
        else if (len > 0) cb(datagram);
        process_size += datagram->size();
    }
    return process_size;
}

}}}

#endif // __PAN_NET_PROTOCOL_UTILS_HPP__