#ifndef CPPWEB_EVENTLOOPTHREAD_HPP
#define CPPWEB_EVENTLOOPTHREAD_HPP

#include "nocopyable.hpp"
#include <thread>
#include <condition_variable>
#include <mutex>

namespace CPPWEB {

class EventLoop;

class EventLoopThread: nocopyable {
public:
    EventLoopThread():m_started(false), m_loop(nullptr) {}
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void runInThread();

private:
    std::thread m_thread;
    std::condition_variable m_cond;
    std::mutex m_mutex;
    bool m_started;
    EventLoop * m_loop;
};

}


#endif