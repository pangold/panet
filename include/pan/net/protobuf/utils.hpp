#ifndef __PAN_NET_PROTOBUF_UTILS_HPP__
#define __PAN_NET_PROTOBUF_UTILS_HPP__

#include <memory>
#include <google/protobuf/message.h>
#include <pan/net/protobuf/datagram.hpp>


namespace pan { namespace net { namespace protobuf {

// stream to datagram
inline static istream& operator >> (istream& is, datagram& datagram)
{
    is >> datagram.len(); 
    if (is.remaining_size() < static_cast<std::size_t>(datagram.len())) {
        // not having enough data for a complete datagram
        return is;
    }
    // but len type of istream is uint16_t but not int32_t, 
    // or we can use it simply like this
    // is >> datagram.name; 
    datagram::namelen_type namelen;
    is >> namelen;
    datagram.name().resize(namelen);
    is.read(&datagram.name()[0], static_cast<std::size_t>(namelen));

    datagram.data().resize(datagram.len() - datagram.name().size() - sizeof(datagram::namelen_type) - sizeof(datagram.checksum()), 0);
    is.read(&datagram.data()[0], datagram.data().size());
    is >> datagram.checksum();

    return is;
}

// datagram to stream
inline static ostream& operator << (ostream& os, const datagram& datagram)
{
    os << datagram.len();
    os << static_cast<int32_t>(datagram.name().size());
    os.write(datagram.name().c_str(), datagram.name().size());
    os.write(datagram.data().c_str(), datagram.data().size());
    os << datagram.calculate_checksum(); // make sure checksum is valid
    return os;
}

// serious error
class data_error : public std::runtime_error {
public:
    data_error()
        : std::runtime_error("data error")
    { }
};

typedef std::shared_ptr<google::protobuf::Message> message_ptr;

// create by name
inline static google::protobuf::Message* create_protobuf_message(const std::string& type_name)
{
    using namespace google::protobuf;
    Message* message = NULL;
    const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if (descriptor) {
        const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}

// stream to datagram with error
inline static datagram_ptr parse_to_datagram(istream& is)
{
    auto pdatagram = std::make_shared<datagram>();
    is >> *pdatagram;
    if (!pdatagram->complete()) {
        return nullptr;
    }
    if (!pdatagram->valid()) {
        throw data_error();
    }
    return pdatagram;
}

// datagram -> message
inline static message_ptr parse_to_message(datagram_ptr datagram)
{
    message_ptr message(create_protobuf_message(datagram->name()));
    if (!message->ParseFromString(datagram->data())) {
        std::string msg = "parse error, message name: " + datagram->name();
        throw std::exception(msg.c_str());
    }
    return message;
}

// message -> datagram
inline static datagram_ptr serialize_from_message(message_ptr message)
{
    std::string data;
    if (!message->SerializePartialToString(&data)) {
        std::string msg = "serialize error, message: " + message->DebugString();
        throw std::exception(msg.c_str());
    }
    return std::make_shared<datagram>(message->GetTypeName() + "\0", data);
}

// stream -> datagram -> message
inline static istream& operator >> (istream& is, google::protobuf::Message*& message)
{
    if (auto datagram = parse_to_datagram(is)) {
        message = create_protobuf_message(datagram->name());
        if (!message->ParseFromString(datagram->data())) {
            std::string msg = "parse error, message name: " + datagram->name();
            throw std::exception(msg.c_str());
        }
    }
    return is;
}

// message -> datagram -> stream
inline static ostream& operator << (ostream& os, const google::protobuf::Message* message)
{
    std::string data;
    if (!message->SerializePartialToString(&data)) {
        std::string msg = "serialize error, message: " + message->DebugString();
        throw std::exception(msg.c_str());
    }
    os << datagram(message->GetTypeName() + "\0", data);
    return os;
}

}}}

#endif // __PAN_NET_PROTOBUF_UTILS_HPP__