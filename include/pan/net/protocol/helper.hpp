#ifndef __PAN_NET_PROTOBUF_HELPER_HPP__
#define __PAN_NET_PROTOBUF_HELPER_HPP__

#include <google/protobuf/message.h>

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

}}}

#endif // __PAN_NET_PROTOBUF_HELPER_HPP__