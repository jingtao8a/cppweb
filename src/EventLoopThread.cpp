#include "EventLoopThread.hpp"
#include "EventLoop.hpp"
#include <assert.h>
#include "Logger.hpp"
#include "Singleton.hpp"


namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

EventLoopThread::~EventLoopThread() {
    if (m_started) {
        m_thread.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!m_started);
    m_started = true;
    m_thread = std::thread([this](){ this->runInThread(); });
    std::unique_lock<std::mutex> guard(m_mutex);
    m_cond.wait(guard);
    return m_loop;//返回在新线程里创建的EventLoop
}

void EventLoopThread::runInThread() {
    EventLoop loop;//在新的线程里创建EventLoop
    m_loop = &loop;
    m_cond.notify_one();
    loop.loop();
}

}