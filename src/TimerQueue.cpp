#include "TimerQueue.hpp"
#include <sys/timerfd.h>
#include <unistd.h>
#include <strings.h>
#include "Logger.hpp"
#include "Singleton.hpp"
#include "EventLoop.hpp"

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

static int timerfdCreate() {
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd == -1) {
        SYSFATAL(logger, "timer_create()");
    }
    return fd;
}

static void timerfdRead(int fd) {
    uint64_t val;
    size_t n = read(fd, &val, sizeof(val));
    if (n != sizeof(val)) {
        ERROR(logger, "timerfdRead get %ld, not %lu", n, sizeof(val));
    }
}

static struct timespec durationFromNow(TimePoint when) {
    struct timespec ret;
    Nanoseconds ns = when - clock::now();
    if (ns < std::chrono::milliseconds(1)) {//不足1ms 以1ms计算
        ns = std::chrono::milliseconds(1);
    }
    ret.tv_sec = static_cast<time_t>(ns.count() / std::chrono::nanoseconds::period::den);
    ret.tv_nsec = ns.count() % std::chrono::nanoseconds::period::den;
    return ret;
}


static void timerfdSet(int fd, TimePoint when) {
    struct itimerspec oldtime, newtime;
    bzero(&oldtime, sizeof(oldtime));
    bzero(&newtime, sizeof(newtime));
    newtime.it_value = durationFromNow(when);
    int ret = timerfd_settime(fd, 0, &newtime, &oldtime);
    if (ret == -1) {
        SYSERR(logger, "timerfd_settime()");
    }
}

TimerQueue::TimerQueue(EventLoop *loop) :
    m_loop(loop),
    m_timerFd(timerfdCreate()),
    m_timerChannel(loop, m_timerFd) {

    m_loop->assertInLoopThread();
    m_timerChannel.setReadCallback( [this](){this->handleRead(); });
    m_timerChannel.enableRead();
}

TimerQueue::~TimerQueue() {
    for (auto& p : m_timers) {
        delete p.second;
    }
    ::close(m_timerFd);
}

Timer* TimerQueue::addTimer(std::function<void()> cb, TimePoint when, Nanoseconds interval) {
    Timer* timer = new Timer(cb , when, interval);
    m_loop->runInLoop([=]() {
        auto ret = m_timers.insert({when, timer});
        assert(ret.second);
        if (m_timers.begin() == ret.first) {
            timerfdSet(m_timerFd, when);
        }
    });
    
    return timer;
}


void TimerQueue::cancelTimer(Timer *timer) {
    m_loop->runInLoop([timer, this]() {
        timer->cancel();
        this->m_timers.erase({timer->when(), timer});
        delete timer;
    });
}

void TimerQueue::handleRead() {
    m_loop->assertInLoopThread();
    timerfdRead(m_timerFd);

    TimePoint now(clock::now());
    for (auto& e : getExpired(now)) {
        Timer* timer = e.second;
        assert(timer->expired(now));
        if (!timer->isCanceled()) {
            timer->run();
        }
        if (!timer->isCanceled() && timer->isRepeat()) {
            timer->restart();
            e.first = timer->when();
            m_timers.insert(e);
        } else {
            delete timer;
        }
    }

    if (!m_timers.empty()) {
        timerfdSet(m_timerFd, m_timers.begin()->first);
    }
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimePoint now) {
    Entry en(now + std::chrono::nanoseconds(1), nullptr);
    std::vector<Entry> entries;
    auto end = m_timers.lower_bound(en);
    entries.assign(m_timers.begin(), end);
    m_timers.erase(m_timers.begin(), end);
    return entries;
}


}