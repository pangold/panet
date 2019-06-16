#ifndef __PAN_NET_TCP_SESSION_POOL_HPP__
#define __PAN_NET_TCP_SESSION_POOL_HPP__

#include <pan/net/tcp/session.hpp>

namespace pan { namespace net { namespace tcp {
    
template <typename Handler>
class session_pool {
public:
    typedef Handler handler_type;
    typedef session<handler_type> session_type;
    typedef typename session_type::pointer session_ptr;

    ~session_pool() { }

    void insert(session_ptr session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_[session->to_string()] = session;
        cond_.notify_all();
    }

    void remove(session_ptr session)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.erase(session->to_string());
    }

    void clear()
    {
        while (!pool_.empty()) {
            auto it = pool_.rbegin();
            // stop will trigger member function remove(session)
            it->second->stop();
        }
    }

    session_ptr front()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.begin()->second;
    }

    session_ptr operator [] (const std::string& key)
    {
        // that will be safe with lock, but low performance
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_[key];
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.empty();
    }

    // wait for the 1st session
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (pool_.empty()) {
            cond_.wait(lock);
        }
    }
    
    void wait(const std::string& key)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!pool_[key]) {
            cond_.wait(lock);
        }
    }

private:
    std::map<std::string, session_ptr> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;

};

}}}

#endif // __PAN_NET_TCP_SESSION_POOL_HPP__
