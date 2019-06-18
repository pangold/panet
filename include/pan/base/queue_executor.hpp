#ifndef __PAN_QUEUE_EXECUTOR_HPP__
#define __PAN_QUEUE_EXECUTOR_HPP__

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace pan {

class queue_executor {
public:
    queue_executor() 
        : destroy_(false) 
    { }

    virtual ~queueu_executor() 
    { 
        destroy_ = true;  
        cond_.notify_all(); 
    }
    
    template <typename F, typename... Args>
    void push(F func, Args... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push(std::bind(func, std::forward<Args>(args)...));
        cond_.notify_one();
    }
    
    std::function<void()> pop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto func = tasks_.front();
        tasks_.pop();
        return std::move(func);
    }

    std::queue<std::function<void()>> pop_all()
    {
        std::queue<std::function<void()>> temp;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::swap(temp, tasks_);
        }
        return std::move(temp);
    }
    
    void wait()
    {
        while (tasks_.empty() && !destroy_) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock);
        }
    }
    
    template <typename Pred>
    void wait(Pred pred)
    {
        while (tasks_.empty() && !destroy_ && pred()) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock);
        }
    }
    
private:
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool destroy_;
    
};

}

#endif // __PAN_QUEUE_EXECUTOR_HPP__