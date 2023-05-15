#ifndef CPPWEB_CHANNEL_HPP
#define CPPWEB_CHANNEL_HPP

#include <functional>
#include <sys/epoll.h>
#include <memory>
#include "nocopyable.hpp"

namespace CPPWEB {
class EventLoop;

class Channel : public nocopyable {
public:
    Channel(EventLoop* loop, int fd);

    void setReadCallback(const std::function<void()>& cb) { m_readCallback = cb; }
    void setWriteCallback(const std::function<void()>& cb) { m_writeCallback = cb; }
    void setCloseCallback(const std::function<void()>& cb) { m_closeCallback = cb; }
    void setErrorCallback(const std::function<void()>& cb) { m_errorCallback = cb; }
    
    bool isReading() const { return m_events & EPOLLIN; }
    bool isWriting() const { return m_events & EPOLLOUT; }
    bool isNoneEvents() const { return m_events == 0;}

    int getFd() const { return m_fd; }
    unsigned getEvents() const { return m_events; }
    void setRevents(unsigned events) { m_revents = events;}

    void enableRead() { m_events |= (EPOLLIN | EPOLLPRI); update(); }
    void enableWrite() { m_events |= EPOLLOUT; update(); }
    void disableRead() { m_events |= ~EPOLLIN; update(); }
    void disableWrite() { m_events |= ~EPOLLOUT; update(); }
    void disableAll() { m_events = 0; update(); }

    void handleEvents();
    void tie(const std::shared_ptr<void>& obj);
private:
    void update();
    void handleEventsWithGuard();
    
private:
    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    std::function<void()> m_closeCallback;
    std::function<void()> m_errorCallback;

    int m_fd;
    unsigned m_events;
    unsigned m_revents;
    EventLoop* m_loop;
    
    bool m_tied;
    std::weak_ptr<void> m_tie;
public:
    
    bool polling;
};

}


#endif