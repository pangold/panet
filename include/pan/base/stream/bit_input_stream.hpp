#ifndef __PAN_BIT_ISTREAM_HPP__
#define __PAN_BIT_ISTREAM_HPP__

#include <cstdint>
#include <vector>
#include <string>
#include <list>
#include <tuple>
#include <cassert>
#include <pan/base/endianness.hpp>

namespace pan { namespace bit {

class istream {
public:
    istream(const void* buffer, std::size_t size)
        : buffer_((char*)buffer)
        , head_(0)
        , capacity_(size * 8)
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
        return (head_ + 7) >> 3;
    }

    std::size_t capacity() const
    {
        return (capacity_ + 7 ) >> 3;
    }

    std::size_t remaining_size() const
    {
        return capacity() - size();
    }

    void read_bits(std::uint8_t& out, std::size_t bit_count)
    {
        assert(bit_count <= 8);
        std::size_t result_size = head_ + bit_count;
        if (result_size > capacity_) {
            // throw std::exception("memory overread");
            return;
        }
        std::uint32_t byte_offset = head_ >> 3;
        std::uint32_t bit_offset = head_ & 0x07;
        std::uint32_t next_bit_offset = (bit_offset + bit_count) & 0x07;
        std::uint32_t bits_free_this_byte = 7 - bit_offset;
        std::uint8_t current_mask = 0xff << bit_offset;
        std::uint8_t next_mask = ~(0xff << next_bit_offset);
        if (bits_free_this_byte < bit_count) {
            out = (buffer_[byte_offset] & current_mask) >> bit_offset | 
                (buffer_[byte_offset + 1] & next_mask) << (bit_count - next_bit_offset);
        } else {
            out = (buffer_[byte_offset] & current_mask & next_mask) >> bit_offset;
        }
        head_ = result_size;
    }

    void read_bits(void* out, std::size_t bit_count)
    {
        std::uint8_t* src_byte = static_cast<std::uint8_t*>(out);
        while (bit_count >= 8) {
            read_bits(*src_byte, 8);
            src_byte++;
            bit_count -= 8;
        }
        if (bit_count > 0) {
            read_bits(*src_byte, bit_count);
        }
    }

    void read_bytes(void* out, std::size_t byte_count)
    {
        read_bits(out, byte_count << 3);
    }

    void read(bool& out)
    {
        read_bits(&out, 1);
    }

    template <typename T>
    void read(T& out, std::size_t bit_count = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "generic read only supports primitive data types");
        read_bits(&out, bit_count);
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


inline static bit::istream& operator >> (bit::istream& is, std::string& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    is.read_bits(&out[0], static_cast<std::size_t>(length * 8));
    return is;
}

template <typename T>
inline static bit::istream& operator >> (bit::istream& is, std::vector<T>& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    for (T& element : out) {
        is.read(element);
    }
    return is;
}

template <typename T>
inline static bit::istream& operator >> (bit::istream& is, std::list<T>& out)
{
    std::uint16_t length;
    is.read(length);
    out.resize(length);
    for (T& element : out) {
        is.read(element);
    }
    return is;
}

} // namespace bit

template <typename Tuple, std::size_t... Ids>
inline static Tuple istream_to_tuple(bit::istream& istream, std::index_sequence<Ids...>)
{
    Tuple tuple;
    std::initializer_list<int>{((istream >> std::get<Ids>(tuple)), 0)...};
    return tuple;
}

template <typename Tuple>
inline static Tuple istream_to_tuple(bit::istream& istream)
{
    constexpr auto N = std::tuple_size<typename std::decay<Tuple>::type>::value;
    return istream_to_tuple<Tuple>(istream, std::make_index_sequence<N>{});
}

} // namespace pan

#endif // __PAN_NET_BIT_ISTREAM_HPP__