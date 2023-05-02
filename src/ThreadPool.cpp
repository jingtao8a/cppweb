#include "ThreadPool.hpp"
#include <assert.h>
#include "Logger.hpp"
#include "Singleton.hpp"

namespace CPPWEB {

auto& logger = Singleton<Logger>::GetInstance();

ThreadPool::ThreadPool(size_t numThread, size_t maxQueueSize, const std::function<void(size_t index)>& cb) :
    m_maxQueueSize(maxQueueSize), 
    m_threadInitCallback(cb),
    m_running(true) {
    assert(maxQueueSize > 0);
    for (size_t i = 1; i <= numThread; ++i) {
        m_threadList.emplace_back(new std::thread([this, i]() { this->runInThread(i); }));
    }
    TRACE(logger, "ThreadPool() numThreads %lu, maxQueueSize %lu", numThread, maxQueueSize);
}

ThreadPool::~ThreadPool() {
    if (m_running) {
        stop();
    }
    TRACE(logger, "~ThreadPool()");
}

void ThreadPool::stop() {
    assert(m_running);
    while (true) {//保证stop时所有task取完
        std::lock_guard<std::mutex> guard(m_mutex);
        if (m_taskQueue.empty()) {
            break;
        }
    }
    m_running = false;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_notEmpty.notify_all();//解除线程阻塞状态
    }
    for (auto& thread : m_threadList) {
        thread->join();//回收线程
    }
}

void ThreadPool::runTask(const std::function<void()>& task) {
    assert(m_running);
    if (m_threadList.empty()) {
        task();
    } else {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_taskQueue.size() >= m_maxQueueSize) {
            m_notFull.wait(lock);
        }
        m_taskQueue.push_back(task);
        m_notEmpty.notify_one();
    }
}

void ThreadPool::runInThread(size_t index) {
    if (m_threadInitCallback) {
        m_threadInitCallback(index);
    }

    while (m_running) {
        if (auto task = takeTask()) {
            task();
        }
    }
}

std::function<void()> ThreadPool::takeTask() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_taskQueue.empty() && m_running) {
        m_notEmpty.wait(lock);
    }
    std::function<void()> task;
    if (!m_taskQueue.empty()) {
        task = m_taskQueue.front();
        m_taskQueue.pop_front();
        m_notFull.notify_one();
    }
    return task;
}
}
