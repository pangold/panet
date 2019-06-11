#ifndef __PAN_QUEUE_EXECUTOR_HPP__
#define __PAN_QUEUE_EXECUTOR_HPP__

class queue_executor {
public:
    queue_executor();
    
    template <typename Function, typename... Args>
    void push(Function func, Args... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.size() > max_task_count_) {
            throw maximum_task_exception();
        }
        tasks_.push(std::bind(&thread::run_task<Function, Args...>, this, func, std::forward<Args>(args)...));
        cond_.notify_one();
    }
    
    std::function<void()> pop()
    {
    }
    
    void wait_for()
    {
        while (tasks_.empty() && running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock);
        }
    }
    
private:
    

    template <typename Function, typename... Args>
    void run_task(Function func, Args... args)
    {
        run_task_impl(func, std::forward<Args>(args)...);
    }

    template <typename Result, typename... Args>
    void run_task_impl(std::function<Result(Args...)> func, Args... args) 
    {
        func(std::forward<Args>(args)...);
    }

    template <typename Result, typename... Args>
    void run_task_impl(Result(*func)(Args...), Args... args) 
    {
        func(std::forward<Args>(args)...);
    }

    template <typename Result, typename Class, typename Object, typename... Args>
    void run_task_impl(Result(Class::* func)(Args...), Object* obj, Args... args) 
    {
        (obj->*func)(std::forward<Args>(args)...);
    }
    
private:
    std::queue<std::function<void()> > tasks_;
    
};

#endif // __PAN_QUEUE_EXECUTOR_HPP__