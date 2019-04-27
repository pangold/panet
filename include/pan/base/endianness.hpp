#ifndef __PAN_NET_ENDIANNESS_HPP__
#define __PAN_NET_ENDIANNESS_HPP__

#include <cstdint>

#define BIG_ENDIANNESS        0
#define SMALL_ENDIANNESS      1
#define STREAM_ENDIANNESS     BIG_ENDIANNESS
// FIXME: according to the real value of the platform
#define PLATFORM_ENDIANNESS   BIG_ENDIANNESS

namespace pan { namespace net {

// for handling a data with 2 bytes
inline std::uint16_t byte_swap_2(std::uint16_t data)
{
    return (data >> 8) | (data << 8);
}

// for handling a data with 4 bytes
inline std::uint32_t byte_swap_4(std::uint32_t data)
{
    return ((data >> 24) & 0x000000ff) |
        ((data >> 8)  & 0x0000ff00) | 
        ((data << 8)  & 0x00ff0000) | 
        ((data << 24) & 0xff000000);
}

// for handling a data with 8 bytes
inline std::uint64_t byte_swap_8(std::uint64_t data)
{
    return ((data >> 56) & 0x00000000000000ff) | 
        ((data >> 40) & 0x000000000000ff00) |
        ((data >> 24) & 0x0000000000ff0000) | 
        ((data >> 8)  & 0x00000000ff000000) | 
        ((data << 8)  & 0x000000ff00000000) | 
        ((data << 24) & 0x0000ff0000000000) | 
        ((data << 40) & 0x00ff000000000000) | 
        ((data << 56) & 0xff00000000000000);
}

// specialize for handling double, float, signed integers, large enums, etc.
template <typename From, typename To>
class type_aliaser {
public:
    type_aliaser(From in_from_value) : as_from_type(in_from_value) { }
    To& get() { return as_to_type; }
    union {
        From as_from_type;
        To as_to_type;
    };
};

template <typename T, std::size_t TSize> 
class byte_swapper {
public:
    T swap(T data) const
    {
        return data;
    }
};

template <typename T>
class byte_swapper<T, 2> {
public:
    T swap(T data) const 
    {
        std::uint16_t result = byte_swap_2(type_aliaser<T, std::uint16_t>(data).get());
        return type_aliaser<std::uint16_t, T>(result).get();
    }
};

template <typename T>
class byte_swapper<T, 4> {
public:
    T swap(T data) const 
    {
        std::uint32_t result = byte_swap_4(type_aliaser<T, std::uint32_t>(data).get());
        return type_aliaser<std::uint32_t, T>(result).get();
    }
};

template <typename T>
class byte_swapper<T, 8> {
public:
    T swap(T data) const 
    {
        std::uint64_t result = byte_swap_8(type_aliaser<T, std::uint64_t>(data).get());
        return type_aliaser<std::uint64_t, T>(result).get();
    }
};

template <typename T>
inline T byte_swap(T data)
{
    static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, 
        "only supports primitive data types");
    return byte_swapper<T, sizeof(T)>().swap(data);
}

template <typename T>
inline T swap_helper(T data)
{
    if (STREAM_ENDIANNESS != PLATFORM_ENDIANNESS) {
        data = byte_swap(data);
    }
    return data;
}

}}

#endif // __PAN_NET_ENDIANNESS_HPP__