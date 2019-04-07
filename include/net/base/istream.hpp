#ifndef __PAN_NET_ISTREAM_HPP__
#define __PAN_NET_ISTREAM_HPP__

#include <cstdint>
#include <vector>
#include <string>
#include <list>
#include <tuple>
#include <net/base/endianness.hpp>

namespace pan { namespace net {

class istream {
public:
    // do not own the memory
    istream(const void* buffer, std::size_t size)
        : buffer_((char*)buffer)
        , head_(0)
        , capacity_(size)
    {

    }

    ~istream()
    {

    }

    const char* data() const
    {
        return buffer_;
    }

    std::size_t size() const
    {
        return head_;
    }

    std::size_t capacity() const
    {
        return capacity_;
    }

    std::size_t remaining_size() const
    {
        return capacity() - size();
    }

    void read(void* out, std::size_t size)
    {
        std::size_t result_size = head_ + size;
        if (result_size > capacity_) {
            // throw std::exception("memory overread");
            return;
        }
        std::memcpy(out, buffer_ + head_, size);
        head_ = result_size;
    }

    template <typename T>
    void read(T& out)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "generic read only supports primitive data types");
        read(&out, sizeof(out));
        out = swap_helper(out);
    }

    template <typename T>
    istream& operator >> (T& out)
    {
        read(out);
        return *this;
    }

private:
    const char* buffer_;
    std::size_t head_;
    std::size_t capacity_;

};


inline static istream& operator >> (istream& is, std::string& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    is.read(&out[0], static_cast<std::size_t>(length));
    return is;
}

template <typename T>
inline static istream& operator >> (istream& is, std::vector<T>& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    for (T& element : out) {
        is.read(element);
    }
    return is;
}

template <>
inline static istream& operator >> (istream& is, std::vector<char>& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    is.read(&out[0], static_cast<std::size_t>(length));
    return is;
}

template <typename T>
inline static istream& operator >> (istream& is, std::list<T>& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    for (T& element : out) {
        is.read(element);
    }
    return is;
}

template <typename Tuple, std::size_t... Ids>
inline static Tuple istream_to_tuple(istream& istream, std::index_sequence<Ids...>)
{
    Tuple tuple;
    std::initializer_list<int>{((istream >> std::get<Ids>(tuple)), 0)...};
    return tuple;
}

template <typename Tuple>
inline static Tuple istream_to_tuple(istream& istream)
{
    constexpr auto N = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return istream_to_tuple<Tuple>(istream, std::make_index_sequence<N>{});
}

}}

#endif // __PAN_NET_ISTREAM_HPP__