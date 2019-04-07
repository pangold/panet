#ifndef __PAN_MAP_HPP__
#define __PAN_MAP_HPP__

#include <unordered_map>
#include <mutex>
#include <condition_variable>

namespace pan {

template <typename Key, 
    typename Value, 
    typename Container = std::unordered_map<Key, Value> >
class map {
public:
    using key_type = Key;
    using type = Value;
    using container_type = Container;
public:
    virtual ~map()
    {
        
    }
    type get(key_type key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = elements_.find(key);
        if (it != elements_.end()) {
            return it->second;
        }
        throw not_found_error(key);
    }
    type pop(key_type key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = elements_.find(key);
        if (it != elements_.end()) {
            type value = it->second;
            elements_.erase(it);
            return value;
        }
        throw not_found_error(key);
    }
    void insert(key_type key, const type& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        elements_[key] = value;
        cond_.notify_one();
    }
    void remove(key_type key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = elements_.find(key);
        if (it != elements_.end()) {
            elements_.erase(it);
        }
    }
    bool has(key_type key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return elements_.count(key) != 0;
    }
    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        elements_.clear();
    }
    bool empty() 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return elements_.empty();
    }
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock);
    }
    void wait(std::size_t timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait_for(lock, std::chrono::seconds(timeout));
    }
    type wait_for(key_type key)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (elements_.count(key) == 0) {
            cond_.wait(lock);
        }
        // FIXME: search extra two times
        type value = elements_[key];
        elements_.erase(key);
        return value;
    }
    type wait_for(key_type key, std::size_t timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto pred = [this, key]() { return elements_.count(key) != 0; };
        cond_.wait_for(lock, std::chrono::milliseconds(timeout), pred);
        if (pred()) {
            type value = elements_[key];
            elements_.erase(key);
            return value;
        }
        throw timeout_error(key);
    }
    template <typename Operation>
    void action(Operation operation)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& element : elements_) {
            operation(element.second);
        }
    }
    template <typename Condition, typename Operation>
    void action(Condition condition, Operation operation)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& element : elements_) {
            if (condition(element.second)) {
                operation(element.second);
            }
        }
    }
    template <typename Condition>
    void filter(Condition condition)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = elements_.begin(); it != elements_.end(); ) {
            if (condition(it->second)) {
                it = elements_.erase(it);
            } else {
                it++;
            }
        }
    }
private:
    container_type elements_;
    std::condition_variable cond_;
    std::mutex mutex_;

public:
    class timeout_error : public std::runtime_error {
    public:
        timeout_error(key_type key)
            : std::runtime_error("wait for key [ " + std::to_string(key) + " ] timeout")
        { }
    };

    class not_found_error : public std::runtime_error {
    public:
        not_found_error(key_type key)
            : std::runtime_error("key [ " + std::to_strong(key) + " ] is not found")
        { }
    };

};

}

#endif // __PAN_MAP_HPP__
