#ifndef __PAN_NET_RPC_SEQUENCE_HPP__
#define __PAN_NET_RPC_SEQUENCE_HPP__

#include <cstdint>
#include <mutex>
#include <bitset>

namespace pan { namespace net { namespace rpc {

template <std::size_t MaxRequest>
class sequence {
    template <std::size_t MaxRequest> friend class sequence_guard;

public:
    sequence()
        : sequence_(0)
        , mutex_()
        , requests_()
    { }

    bool test(std::size_t seq)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return requests_.test(seq);
    }

private:
    std::uint16_t allocate()
    {
        std::size_t first = sequence_ % MaxRequest;
        std::size_t result = first;
        std::lock_guard<std::mutex> lock(mutex_);
        while (requests_.test(result)) {
            ++result;
            result %= MaxRequest;
            if (result == first) {
                throw std::exception("reached maximum number of requests");
            }
        }
        sequence_ = result + 1;
        requests_.set(result);
        return static_cast<std::uint16_t>(result);
    }

    void release(std::size_t seq)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        requests_.reset(seq);
    }

private:
    std::size_t sequence_;
    std::mutex mutex_;
    std::bitset<MaxRequest> requests_;

};

template <std::size_t MaxRequest>
class sequence_guard {
public:
    sequence_guard(sequence<MaxRequest>& sequence)
        : sequencer_(sequence)
        , index_(sequencer_.allocate())
    { }

    ~sequence_guard()
    { 
        sequencer_.release(index_); 
    }

public:
    std::size_t number() const 
    { 
        return index_ + 1; 
    }

    std::size_t index() const 
    { 
        return index_; 
    }
    
private:
    sequence<MaxRequest>& sequencer_;
    std::size_t index_;

};

}}}

#endif // __PAN_NET_RPC_SEQUENCE_HPP__