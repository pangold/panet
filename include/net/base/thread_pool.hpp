#ifndef __PAN_THREAD_POOL_HPP__
#define __PAN_THREAD_POOL_HPP__

#include <ctime>
#include <vector>
#include <net/base/log.hpp>
#include <net/base/noncopyable.hpp>
#include <net/base/thread.hpp>

namespace pan {

class thread_pool : public noncopyable {
public:
    explicit thread_pool(std::size_t thread_count = 2, std::size_t max_task_per_thread = 1024)
        : threads_()
        , max_task_per_thread_(max_task_per_thread)
    {
        resize(thread_count);
    }

    virtual ~thread_pool()
    {
        LOG_INFO("thread pool destroy");
        clear();
    }

    template <typename Function, typename... Args>
    void dispatch(Function func, Args... args)
    {
        std::srand(unsigned(std::time(0)));
        auto index = static_cast<std::size_t>(std::rand() % threads_.size());
        threads_[index]->push(func, std::forward<Args>(args)...);
    }

    void resize(std::size_t expected_count)
    {
        while (threads_.size() > expected_count) {
            // threads_.front()->stop();
            threads_.pop_front();
        }
        while (threads_.size() < expected_count) {
            auto pt = std::make_shared<thread>(max_task_per_thread_);
            pt->start();
            threads_.push_back(pt);
        }
    }

    void clear()
    {
        resize(0);
    }

private:
    std::deque<std::shared_ptr<thread> > threads_;
    std::size_t max_task_per_thread_;

};

}

#endif // __PAN_THREAD_POOL_HPP__