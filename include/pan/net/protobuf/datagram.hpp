#ifndef __PAN_NET_PROTOBUF_DATAGRAM_HPP__
#define __PAN_NET_PROTOBUF_DATAGRAM_HPP__

#include <cstdint>
#include <string>
#include <pan/base.hpp>

namespace pan { namespace net { namespace protobuf {

class datagram {
public:
    using namelen_type = std::int32_t;

    datagram() = default;

    datagram(const std::string& name, const std::string& data)
        : len_(name.size() + data.size() + sizeof(checksum_) + sizeof(namelen_type))
        , name_(name)
        , data_(data)
        , checksum_(calculate_checksum())
    { }

    void set_len(int32_t len) noexcept { len_ = len; }
    void set_name(const std::string& name) noexcept { name_ = name; }
    void set_data(const std::string& data) noexcept { data_ = data; }
    void set_checksum(int32_t checksum) noexcept { checksum_ = checksum; }

    const int32_t& len() const noexcept { return len_; }
    const std::string& name() const noexcept { return name_; }
    const std::string& data() const noexcept { return data_; }
    const int32_t& checksum() const noexcept { return checksum_; }

    int32_t& len() noexcept { return len_; }
    std::string& name() noexcept { return name_; }
    std::string& data() noexcept { return data_; }
    int32_t& checksum() noexcept { return checksum_; }

    int32_t calculate_checksum() const 
    {
        uint32_t checksum = 0xffffffff;
        int32_t namelen = static_cast<int32_t>(name_.size());
        checksum = CRC32(checksum, reinterpret_cast<const char*>(&len_), 4);
        checksum = CRC32(checksum, reinterpret_cast<const char*>(&namelen), 4);
        checksum = CRC32(checksum, name_.c_str(), name_.size());
        checksum = CRC32(checksum, data_.c_str(), data_.size());
        return static_cast<int32_t>(checksum);
    }

    int32_t calculate_length() const noexcept
    {
        auto len = name_.size() + data_.size() + sizeof(namelen_type) + sizeof(checksum_);
        return static_cast<int32_t>(len);
    }

    // include len field itself
    int32_t total_length() const noexcept
    {
        return calculate_length() + static_cast<int32_t>(sizeof(len_));
    }

    bool valid() const
    {
        return calculate_checksum() == checksum_;
    }

    bool complete() const noexcept
    {
        return calculate_length() == len_;
    }

private:
    // what if we need heart beat datagram.
    // solution 1:
    // can I just keep name&data_ empty?
    // then, there will be 12 bytes length of a package
    // (name has 4 extra bytes for its length, even though its value is 0)
    // solution 2:
    // set name = "hb" or "heart beat", then there will be 14 or 22 bytes length.
    int32_t len_;
    std::string name_;
    std::string data_;
    int32_t checksum_;

};
typedef std::shared_ptr<datagram> datagram_ptr;

}}}

#endif // __PAN_NET_PROTOBUF_DATAGRAM_HPP__
