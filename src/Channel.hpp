#ifndef CPPWEB_CHANNEL_HPP
#define CPPWEB_CHANNEL_HPP

#include <functional>
#include <nocopyable.hpp>
#include <sys/epoll.h>

namespace CPPWEB {

class Channel : public nocopyable {
public:
    Channel();
    ~Channel();

    void setReadCallback(const std::function<void()>& cb) {
        m_readCallback = cb;
    }

    void setWriteCallback(const std::function<void()>& cb) {
        m_writeCallback = cb;
    }

    void setCloseCallback(const std::function<void()>& cb) {
        m_closeCallback = cb;
    }

    void setErrorCallback(const std::function<void()>& cb) {
        m_errorCallback = cb;
    }

    int getFd() const { return m_fd; }
    bool isReading() const { return m_events & EPOLLIN; }
    bool isWriting() const { return m_events & EPOLLOUT; }
private:
    std::function<void()> m_readCallback;
    std::function<void()> m_writeCallback;
    std::function<void()> m_closeCallback;
    std::function<void()> m_errorCallback;
    int m_fd;
    unsigned m_events;
};

}


#endif