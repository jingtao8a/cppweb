#ifndef CPPWEB_TIMERQUEUE_HPP
#define CPPWEB_TIMERQUEUE_HPP

#include "nocopyable.hpp"
#include "Timer.hpp"
#include "Channel.hpp"
#include <set>
#include <memory>
#include <vector>

namespace CPPWEB {

class TimerQueue : public nocopyable {
public:
    typedef std::pair<TimePoint, Timer *> Entry;

public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    Timer* addTimer(std::function<void()> cb, TimePoint when, Nanoseconds interval);
    void cancelTimer(Timer* timer);

private:
    void handleRead();
    std::vector<Entry> getExpired(TimePoint now);

private:
    EventLoop* m_loop;
    const int m_timerFd;
    Channel m_timerChannel;
    std::set<Entry> m_timers;//按照Entry中的第一个元素，也就是TimePoint比大小
};

}

#endif