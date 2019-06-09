#ifndef __PAN_NET_PROTOBUF_HELPER_HPP__
#define __PAN_NET_PROTOBUF_HELPER_HPP__

#include <google/protobuf/message.h>
#include <pan/net/protocol.hpp>

namespace pan { namespace net { namespace protobuf {

typedef google::protobuf::Message message_type;
typedef google::protobuf::Descriptor descriptor_type;
typedef google::protobuf::DescriptorPool descriptor_pool;
typedef google::protobuf::MessageFactory message_factory;
typedef std::shared_ptr<message_type> message_ptr;

inline static message_ptr create_protobuf_message(const std::string& type_name)
{
    auto descriptor = descriptor_pool::generated_pool()->FindMessageTypeByName(type_name);
    if (descriptor) {
        auto prototype = message_factory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            return message_ptr(prototype->New());
        }
    }
    return message_ptr();
}

inline static bool message_to_datagram(protocol::datagram& datagram, message_ptr message)
{
    if (!message->SerializePartialToString(&datagram.data()))
        return false;
    datagram.set_data_size(datagram.data().size());
    datagram.set_name(message->GetTypeName() + "\0");
    datagram.set_name_size(datagram.name().size());
    datagram.set_length(datagram.csize());
    datagram.set_checksum(datagram.ccrc());
    return true;
}

inline static bool datagram_to_message(const protocol::datagram& datagram, message_ptr& message)
{
    auto temp = create_protobuf_message(datagram.name());
    if (temp->ParseFromString(datagram.data())) {
        std::swap(message, temp);
        return true;
    }
    return false;
}

inline static int parse(message_ptr& message, const void* data, size_t size)
{
    protocol::datagram datagram;
    auto len = protocol::data_to_datagram(datagram, data, size);
    if (len > 0 && !datagram_to_message(datagram, message)) {
        LOG_ERROR("protobuf.parse: parse %s error", datagram.name().c_str());
        return -1;
    }
    return len;
}

inline static bool serialize(std::string& str, message_ptr message)
{
    protocol::datagram datagram;
    datagram.set_id(std::chrono::steady_clock::now().time_since_epoch().count() / 1000);
    if (!message_to_datagram(datagram, message)) {
        LOG_ERROR("protobuf.serialize: message %s", message->DebugString().c_str());
        return false;
    }
    return protocol::datagram_to_data(str, datagram);
}

}}}

#endif // __PAN_NET_PROTOBUF_HELPER_HPP__