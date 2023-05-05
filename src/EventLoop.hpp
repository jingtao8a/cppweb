#ifndef CPPWEB_EVENTLOOP_HPP
#define CPPWEB_EVENTLOOP_HPP

#include "nocopyable.hpp"
#include "Epoller.hpp"
#include "TimerQueue.hpp"
#include <atomic>
#include <mutex>

namespace CPPWEB {
class Channel;

class EventLoop : public nocopyable {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(const std::function<void()>& task);
    
    void wakeup();

    void updateChannel(Channel *channel);

    void assertInLoopThread();
    void assertNotInLoopThread();
    bool isInLoopThread();

    Timer* runAt(TimePoint when, std::function<void()> cb);
    Timer* runEvery(Nanoseconds interval, std::function<void()> cb);
    Timer* runAfter(Nanoseconds interval, std::function<void()> cb);

    void cancelTimer(Timer* timer);
private:
    void handleRead();

private:
    const pid_t m_tid;
    std::atomic_bool m_quit;
    
    Epoller m_poller;
    std::vector<Channel* > m_activeChannels;

    std::mutex m_mutex;
    std::vector<std::function<void()> > m_pendingTasks;

    TimerQueue m_timerQueue;

    const int m_wakeupFd;
    Channel m_wakeupChannel;
};

}


#endif