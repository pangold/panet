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

}}}

#endif // __PAN_NET_PROTOCOL_UTILS_HPP__