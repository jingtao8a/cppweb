#ifndef CPPWEB_THREADPOOL_HPP
#define CPPWEB_THREADPOOL_HPP

#include "nocopyable.hpp"
#include <functional>
#include <stddef.h>
#include <vector>
#include <memory>
#include <thread>
#include <deque>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace CPPWEB {

class ThreadPool : public nocopyable {
public:
    explicit ThreadPool(size_t numThread, size_t maxQueueSize=65536, const std::function<void(size_t index)>& cb=nullptr);
    ~ThreadPool();
    void stop();
    void runTask(const std::function<void()>& task);

private:
    void runInThread(size_t index);
    std::function<void()> takeTask();

private:
    std::vector<std::unique_ptr<std::thread> > m_threadList;
    
    std::deque<std::function<void()> > m_taskQueue;
    const size_t m_maxQueueSize;
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;
    std::mutex m_mutex;

    std::function<void(size_t index)> m_threadInitCallback;
    std::atomic_bool m_running;
};

}


#endif