#ifndef __PAN_NET_BIT_OSTREAM_HPP__
#define __PAN_NET_BIT_OSTREAM_HPP__

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <tuple>
#include <net/base/endianness.hpp>

namespace pan { namespace net { namespace bit {

class ostream {
public:
    ostream() : buffer_(nullptr), head_(0), capacity_(0)
    { 
        realloc_buffer(256); 
    }

    virtual ~ostream() 
    {
        std::free(buffer_); 
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
        return (capacity_ + 7) >> 3;
    }

    std::size_t remaining_size() const
    {
        return capacity() - size();
    }

    void write_bits(char in, std::size_t bit_count)
    {
        std::uint32_t next_bit_head = head_ + static_cast<std::uint32_t>(bit_count);
        if (next_bit_head > capacity_) {
            realloc_buffer(std::max(capacity_ * 2, next_bit_head));
        }
        std::uint32_t byte_offset = head_ >> 3;
        std::uint32_t bit_offset = head_ & 0x07;
        std::uint32_t bits_free_this_byte = 8 - bit_offset;
        std::uint8_t current_mask = ~(0xff << bit_offset);
        buffer_[byte_offset] = (buffer_[byte_offset] & current_mask) | (in << bit_offset);
        if (bits_free_this_byte < bit_count) {
            buffer_[byte_offset + 1] = in >> bits_free_this_byte;
        }
        head_ = next_bit_head;
    }

    void write_bits(const void* in, std::size_t bit_count)
    {
        const char* src_byte = static_cast<const char*>(in);
        while (bit_count > 8) {
            write_bits(*src_byte, 8);
            src_byte++;
            bit_count -= 8;
        }
        if (bit_count > 0) {
            write_bits(*src_byte, bit_count);
        }
    }

    void write_bytes(const void* in, std::size_t byte_count) 
    { 
        write_bits(in, byte_count << 3); 
    }

    void write(bool in) 
    {
        write_bits(in, 1);
    }

    template <typename T>
    void write(T in, std::size_t bit_count = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "generic write only supports primitive data types");
        auto temp = swap_helper(in);
        write_bits(&temp, bit_count);
    }

    template <typename T>
    ostream& operator << (T t)
    {
        write(t);
        return *this;
    }

private:
    void realloc_buffer(std::size_t new_bit_capacity)
    {
        if (new_bit_capacity == 0) {
            std::free(buffer_);
            buffer_ = nullptr;
        }
        else if (buffer_ = static_cast<char*>(std::realloc(buffer_, (new_bit_capacity + 7) >> 3))) {
            capacity_ = new_bit_capacity;
        }
        else {
            throw std::bad_alloc();
        }
    }

private: 
    char* buffer_;
    std::size_t head_;
    std::size_t capacity_;

};


inline static ostream& operator << (ostream& os, const std::string& in)
{
    std::uint16_t len = static_cast<std::uint16_t>(in.size());
    os.write(len);
    os.write_bytes(in.data(), in.size());
    return os;
}

inline static ostream& operator << (ostream& os, const char* const& in)
{
    os << std::string(in);
    return os;
}

template <typename T>
inline static ostream& operator << (ostream& os, const std::vector<T>& in)
{
    std::uint16_t len = static_cast<std::uint16_t>(in.size());
    os.write(len);
    for (const T& element : in) {
        os.write(element);
    }
    return os;
}

template <>
inline static ostream& operator << (ostream& os, const std::vector<char>& in)
{
    std::uint16_t len = static_cast<std::uint16_t>(in.size());
    os.write(len);
    os.write_bytes(in.data(), in.size());
    return os;
}

template <typename T>
inline static ostream& operator << (ostream& os, const std::list<T>& in)
{
    std::uint16_t len = static_cast<std::uint16_t>(in.size());
    os.write(len);
    for (const T& element : in) {
        os.write(element);
    }
    return os;
}

template <typename Tuple, std::size_t... Is>
inline static void ostream_write_tuple_impl(ostream& os, const Tuple& tuple, std::index_sequence<Is...>)
{
    std::initializer_list<int>{(os.write(std::get<Is>(tuple)), 0)...};
}

template <typename... Args>
inline static ostream& operator << (ostream& os, std::tuple<Args...>& in)
{
    ostream_write_tuple_impl(os, in, std::index_sequence_for<Args...>{});
    return os;
}

template <typename... Args>
inline static void ostream_write_n(ostream&os, Args... args)
{
    using tuple_type = std::tuple<typename std::decay<Args>::type...>;
    tuple_type tuple = std::make_tuple(args...);
    os << tuple;
}

}}}

#endif // __PAN_NET_BIT_OSTREAM_HPP__