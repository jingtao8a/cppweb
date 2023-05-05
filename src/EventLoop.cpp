#include "EventLoop.hpp"
#include "Channel.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

thread_local EventLoop* t_Eventloop = nullptr;

static pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

//避免进程收到SIGPIPE信号直接退出
class IgnoreSigPipe {
public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe ignore;//利用构造函数，在main函数之前执行

EventLoop::EventLoop() :
    m_tid(gettid()),
    m_quit(false),
    m_doingPendingTasks(false),
    m_poller(this),
    m_timerQueue(this),
    m_wakeupFd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
    m_wakeupChannel(this, m_wakeupFd) {

    if (m_wakeupFd == -1) {
        SYSFATAL(logger, "EventLoop::eventFd()");
    }
    m_wakeupChannel.setReadCallback([this](){ this->handleRead(); });
    m_wakeupChannel.enableRead();

    assert(t_Eventloop == nullptr);
    t_Eventloop = this;
}

EventLoop::~EventLoop() {
    assert(t_Eventloop == this);
    t_Eventloop = nullptr;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    size_t n = ::write(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        SYSERR(logger, "EventLoop::wakeup() should ::write() %lu bytes", sizeof(one));
    }
}

void EventLoop::handleRead() {
    uint64_t one;
    size_t n = ::read(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        SYSERR(logger, "EventLoop::handleRead() should ::read %lu bytes", sizeof(one));
    }
}

void EventLoop::loop() {
    assertInLoopThread();
    TRACE(logger, "EventLoop %p polling", this);
    m_quit = false;
    while (!m_quit) {
        m_activeChannels.clear();
        m_poller.poll(m_activeChannels);
        for (auto channel : m_activeChannels) {
            channel->handleEvents();
        }
        doPendingTasks();
    }
    TRACE(logger, "EventLoop %p quit", this);
}

void EventLoop::doPendingTasks() {
    assertInLoopThread();
    std::vector<std::function<void()> > tasks;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        tasks.swap(m_pendingTasks);
    }
    m_doingPendingTasks = true;
    for (auto& task : tasks) {
        task();
    }
    m_doingPendingTasks = false;
}


void EventLoop::quit() {
    assert(!m_quit);
    m_quit = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    assertInLoopThread();
    m_poller.updateChannel(channel);
}


void EventLoop::runInLoop(const std::function<void()>& task) {
    if (isInLoopThread()) {
        task();
    } else {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_pendingTasks.push_back(task);
        }
        wakeup();
    }
}

void EventLoop::assertInLoopThread() {
    assert(isInLoopThread());
}

void EventLoop::assertNotInLoopThread() {
    assert(!isInLoopThread());
}

bool EventLoop::isInLoopThread() {
    return m_tid == gettid();
}

Timer* EventLoop::runAt(TimePoint when, std::function<void()> cb) {
    return m_timerQueue.addTimer(cb, when, Nanoseconds::zero());
}

Timer* EventLoop::runEvery(Nanoseconds interval, std::function<void()> cb) {
    return m_timerQueue.addTimer(cb, clock::nowAfter(interval), interval);
}

Timer* EventLoop::runAfter(Nanoseconds interval, std::function<void()> cb) {
    return runAt(clock::nowAfter(interval), cb);
}

void EventLoop::cancelTimer(Timer* timer) {
    m_timerQueue.cancelTimer(timer);
}

}