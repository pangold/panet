#ifndef __PAN_OSTREAM_HPP__
#define __PAN_OSTREAM_HPP__

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <tuple>
#include <pan/base/endianness.hpp>

namespace pan { 

class ostream {
public:
    ostream() : buffer_(nullptr), head_(0), capacity_(0)
    { 
        realloc_buffer(32); 
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

    void write(const void* data, std::size_t size)
    {
        std::size_t result_head = head_ + size;
        if (result_head > capacity_) {
            realloc_buffer(std::max(capacity_ * 2, result_head));
        }
        std::memcpy(buffer_ + head_, data, size);
        head_ = result_head;
    }

    template <typename T>
    void write(T in)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
            "generic write only supports primitive data types");
        auto temp = swap_helper(in);
        write(&temp, sizeof(temp));
    }

    template <typename T>
    ostream& operator << (T t)
    {
        write(t);
        return *this;
    }

private:
    void realloc_buffer(std::size_t new_length)
    {
        if (new_length == 0) {
            std::free(buffer_);
            buffer_ = nullptr;
        }
        else if (buffer_ = static_cast<char*>(std::realloc(buffer_, new_length))) {
            capacity_ = new_length;
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

// template <typename OutputStream>
inline static ostream& operator << (ostream& os, const std::string& in)
{
    std::uint16_t len = static_cast<std::uint16_t>(in.size());
    os.write(len);
    os.write(in.data(), in.size());
    return os;
}

//template <typename OutputStream>
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
    os.write(in.data(), in.size());
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

}

#endif // __PAN_OSTREAM_HPP__