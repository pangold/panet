#ifndef __PAN_THREAD_HPP__
#define __PAN_THREAD_HPP__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <iostream>
#include <string>
#include <pan/base/noncopyable.hpp>

namespace pan {

class thread : public noncopyable {
public:
    explicit thread(std::size_t max_task_count = 1024)
        : tasks_()
        , max_task_count_(max_task_count)
        , running_(false)
    { }

    virtual ~thread()
    {
        stop();
    }

    void start()
    {
        running_ = true;
        thread_ = std::thread(&thread::run, this);
    }

    void stop()
    {
        running_ = false;
        cond_.notify_all();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    template <typename F, typename... Args>
    void push(F func, Args... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.size() > max_task_count_) {
            throw maximum_task_exception();
        }
        tasks_.push(std::bind(func, std::forward<Args>(args)...));
        cond_.notify_one();
    }

private:
    void run()
    {
        while (running_ || !tasks_.empty()) {
            wait_for_task();
            if (!tasks_.empty()) {
                auto task = tasks_.front();
                tasks_.pop();
                run_one(task);
            }
        }
    }

    void run_one(std::function<void()> task)
    {
        try {
            task();
        }
        catch (const std::exception& e) {
            std::clog << "task.error: " << e.what() << std::endl;
        }
    }

    void wait_for_task()
    {
        while (tasks_.empty() && running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock);
        }
    }

private:
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<std::function<void()> > tasks_;
    std::size_t max_task_count_;
    bool running_;

public:
    class maximum_task_exception : public std::exception {
    public:
        maximum_task_exception()
            : std::exception("reached maximum number of task")
        { }
    };

};

}

#endif // __PAN_THREAD_HPP__