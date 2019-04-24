#ifndef __PAN_NET_RPC_PROTOCOL_HPP__
#define __PAN_NET_RPC_PROTOCOL_HPP__

#include <cstdint>
#include <vector>
#include <memory>

namespace pan { namespace net { namespace rpc {

#define RPC_MAGIC                 0x9412
#define RPC_HEADER_LEN            16

enum rpc_datagram_version {
    RPC_DATAGRAM_V1               = 1,
};

enum rpc_serialization_type {
    RPC_SERIALIZATION_BINARY      = 0,   // BIT_STREAM / BYTE_STREAM
    RPC_SERIALIZATION_JSON        = 1,
    RPC_SERIALIZATION_XML         = 2,
};

enum rpc_heart_beat {
    RPC_HEART_BEAT_OFF            = 0,
    RPC_HEART_BEAT_ON             = 1,
    RPC_HEART_BEAT                = RPC_HEART_BEAT_ON,
};

enum rpc_one_way {
    RPC_ONE_WAY_OFF               = 0,
    RPC_ONE_WAY_ON                = 1,
    RPC_ONE_WAY                   = RPC_ONE_WAY_ON,
};

enum rpc_request_type {
    RPC_REQUEST_OFF               = 0,
    RPC_REQUEST_ON                = 1,
    RPC_REQUEST                   = RPC_REQUEST_ON,
    RPC_RESPOND                   = RPC_REQUEST_OFF,
};

enum rpc_status_code {
    RPC_STATUS_SUCCESS            = 0,         // result is at body
    RPC_STATUS_FAILURE            = 20,        // prompt info at body
    RPC_STATUS_NOT_FOUND,
    RPC_STATUS_TIMEOUT,        
    RPC_STATUS_NOT_READY,
    RPC_STATUS_UNKNOWN_ERROR,
};

struct datagram {
    std::uint16_t magic           = RPC_MAGIC;                  // 2 bytes
    std::uint16_t header_size     = RPC_HEADER_LEN;             // 2 bytes
    std::uint8_t version          = RPC_DATAGRAM_V1;            // 1 byte
    std::uint8_t stype            = RPC_SERIALIZATION_BINARY;   // 5 bits;
    std::uint8_t heart_beat       = RPC_HEART_BEAT_OFF;         // 1 bit;
    std::uint8_t one_way          = RPC_ONE_WAY_OFF;            // 1 bit;
    std::uint8_t request          = RPC_REQUEST_ON;             // 1 bit;
    std::uint8_t status_code      = RPC_STATUS_SUCCESS;         // 1 byte
    std::uint8_t reserved         = 0;                          // 1 byte
    std::uint32_t message_id      = 0;                          // 4 bytes (sequence) 
    std::uint32_t body_size       = 0;                          // 4 butes
    std::vector<char> body;                                     // any bytes
};
typedef std::shared_ptr<struct datagram> datagram_ptr;

inline static datagram_ptr make_datagram()
{
    return std::make_shared<struct datagram>();
}

inline static datagram_ptr make_reply(std::uint32_t message_id, std::uint8_t code)
{
    datagram_ptr datagram = std::make_shared<struct datagram>();
    datagram->message_id = message_id;
    datagram->status_code = code;
    datagram->request = RPC_RESPOND;
    datagram->body_size = 0;
    return datagram;
}

inline static datagram_ptr make_reply(std::uint32_t message_id, std::uint8_t code, const void* data, std::uint32_t size)
{
    datagram_ptr datagram = std::make_shared<struct datagram>();
    datagram->message_id = message_id;
    datagram->status_code = code;
    datagram->request = RPC_RESPOND;
    datagram->body_size = size;
    datagram->body.assign((char*)data, (char*)data + size);
    return datagram;
}

inline static datagram_ptr make_request(std::uint32_t message_id)
{
    datagram_ptr datagram = std::make_shared<struct datagram>();
    datagram->message_id = message_id;
    datagram->request = RPC_REQUEST;
    datagram->body_size = 0;
    return datagram;
}

inline static datagram_ptr make_request(std::uint32_t message_id, const void* data, std::uint32_t size)
{
    datagram_ptr datagram = std::make_shared<struct datagram>();
    datagram->message_id = message_id;
    datagram->request = RPC_REQUEST;
    datagram->body_size = size;
    datagram->body.assign((char*)data, (char*)data + size);
    return datagram;
}

inline static datagram_ptr make_heart_beat()
{
    datagram_ptr datagram = std::make_shared<struct datagram>();
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

#endif // __PAN_NET_RPC_PROTOCOL_HPP__