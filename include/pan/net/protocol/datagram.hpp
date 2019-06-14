#ifndef __PAN_NET_PROTOCOL_DATAGRAM_HPP__
#define __PAN_NET_PROTOCOL_DATAGRAM_HPP__

#include <cstdint>
#include <string>
#include <pan/base.hpp>

namespace pan { namespace net { namespace protocol {

class datagram {
public:
    datagram() = default;
    datagram(const std::string& name, const std::string& data);
    datagram(const void* data, size_t size);

    void set_length(int32_t len) noexcept { length_ = len; }
    void set_id(int64_t id) noexcept { id_ = id; }
    void set_name_size(int32_t size) noexcept { name_size_ = size; }
    void set_name(const std::string& name) noexcept { name_ = name; }
    void set_data_size(int32_t size) noexcept { data_size_ = size; }
    void set_data(const std::string& data) noexcept { data_ = data; }
    void set_checksum(int32_t checksum) noexcept { checksum_ = checksum; }

    const int32_t& length() const noexcept { return length_; }
    const int64_t& id() const noexcept { return id_; }
    const int32_t& name_size() const noexcept { return name_size_; }
    const std::string& name() const noexcept { return name_; }
    const int32_t& data_size() const noexcept { return data_size_; }
    const std::string& data() const noexcept { return data_; }
    const int32_t& checksum() const noexcept { return checksum_; }

    int32_t& length() noexcept { return length_; }
    int64_t& id() noexcept { return id_; }
    int32_t& name_size() noexcept { return name_size_; }
    std::string& name() noexcept { return name_; }
    int32_t& data_size() noexcept { return data_size_; }
    std::string& data() noexcept { return data_; }
    int32_t& checksum() noexcept { return checksum_; }

    // calculate length, checksum, namesize according to current value.
    // they basically use to init/update length, checksum. 
    // and use to verify beside.
    // prefix 'c' means calculate(according to current state)
    int32_t ccrc() const;
    int32_t cidsize() const { return static_cast<int32_t>(sizeof(id_)); }
    int32_t cnamesize() const { return static_cast<int32_t>(name_.size() + sizeof(name_size_)); }
    int32_t cdatasize() const { return static_cast<int32_t>(data_.size()) + sizeof(data_size_); }
    int32_t ccmsize() const { return static_cast<int32_t>(sizeof(checksum_)); }
    int32_t csize() const { return cidsize() + cnamesize() + cdatasize() + ccmsize(); }
    // total size
    int32_t size() const { return csize() + static_cast<int32_t>(sizeof(length_)); }

    // to verify.
    bool valid() const { return ccrc() == checksum_; }
    bool completed() const noexcept { return csize() == length_; }

    // serialize & parse
    bool from(const void* data, size_t size);
    bool from(istream& is);
    bool to(ostream& os) const;
    bool to(std::string& str) const;

private:
    int64_t id_; // id too
    int32_t name_size_;
    std::string name_;
    int32_t data_size_;
    std::string data_;
    int32_t checksum_;
    int32_t length_;

};

datagram::datagram(const std::string& name, const std::string& data)
    : id_(0)
    , name_size_(name.size())
    , name_(name)
    , data_size_(data.size())
    , data_(data)
    , checksum_(ccrc())
    , length_(csize())
{

}

datagram::datagram(const void* data, size_t size)
{
    from(data, size);
}

inline int32_t datagram::ccrc() const
{
    uint32_t checksum = 0xffffffff;
    checksum = CRC32(checksum, reinterpret_cast<const char*>(&length()), 4);
    checksum = CRC32(checksum, reinterpret_cast<const char*>(&name_size()), 4);
    checksum = CRC32(checksum, name().c_str(), name().size());
    checksum = CRC32(checksum, reinterpret_cast<const char*>(&data_size()), 4);
    checksum = CRC32(checksum, data().c_str(), data().size());
    return static_cast<int32_t>(checksum);
}

bool datagram::from(const void* data, size_t size)
{
    istream is(data, size);
    return from(is);
}

bool datagram::from(istream& is)
{
    is.read(length());
    // completed?
    if (is.remaining_size() < static_cast<size_t>(length())) 
        return false;
    is.read(id());
    is.read(name_size());
    name().resize(name_size());
    is.read(&name()[0], name().size());
    is.read(data_size());
    data().resize(data_size());
    is.read(&data()[0], data().size());
    is.read(checksum());
    return true;
}

bool datagram::to(std::string& str) const
{
    ostream os;
    to(os);
    str.assign(os.data(), os.data() + os.size());
    return true;
}

bool datagram::to(ostream& os) const
{
    os.write(length());
    os.write(id());
    os.write(name_size());
    os.write(name().c_str(), name().size());
    os.write(data_size());
    os.write(data().c_str(), data().size());
    os.write(ccrc()); // ccrc()? or checksum()?
    return true;
}

}}}

#endif // __PAN_NET_PROTOCOL_DATAGRAM_HPP__
