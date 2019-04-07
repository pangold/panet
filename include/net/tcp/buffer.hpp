#ifndef __PAN_NET_TCP_BUFFER_HPP__
#define __PAN_NET_TCP_BUFFER_HPP__

#include <vector>
#include <cstdint>
#include <cassert>

namespace pan { namespace net { namespace tcp {

template <std::size_t PageSize>
class buffer {
public:
    buffer() 
        : buffer_(PageSize)
        , hole_index_(0)
        , free_index_(0)
    { 

    }
    buffer(const uint8_t* data, std::size_t size)
        : buffer_(data, data + size)
        , hole_index_(0)
        , free_index_(size)
    {

    }
    void push(const void* data, std::size_t size)
    {
        lengthen(size);
        std::memcpy(buffer_.data() + free_index_, data, size);
        free_index_ += size;
    }
    void push(std::size_t size)
    {
        assert(size <= this->free_size());
        free_index_ += size;
        if (free_index_ > buffer_.size()) {
            free_index_ = buffer_.size();
        }
    }
    void pop(std::size_t size)
    {
        assert(size <= this->size());
        hole_index_ += size;
        if (hole_index_ >= free_index_) {
            hole_index_ = free_index_ = 0;
        }
    }
    void lengthen(std::size_t size = PageSize)
    {
        if (this->free_size() < size) {
            buffer_.resize((buffer_.size() / PageSize + 1) * PageSize);
        }
    }
    uint8_t* data() 
    { 
        return buffer_.data() + hole_index_; 
    }
    const uint8_t* data() const
    {
        return buffer_.data() + hole_index_;
    }
    uint8_t* free_data()
    {
        return buffer_.data() + free_index_;
    }
    const uint8_t* free_data() const
    { 
        return buffer_.data() + free_index_;
    }
    std::size_t size() const 
    { 
        return free_index_ - hole_index_; 
    }
    std::size_t free_size() const
    {
        return buffer_.size() - free_index_;
    }
    std::size_t capacity() const 
    { 
        return buffer_.size(); 
    }
    bool full() const
    {
        return free_index_ == buffer_.size();
    }
    bool empty() const
    {
        return free_index_ == 0;
    }
private:
    std::vector<uint8_t> buffer_;
    volatile std::size_t hole_index_;
    volatile std::size_t free_index_;
};

}}}

#endif // __PAN_NET_TCP_BUFFER_HPP__