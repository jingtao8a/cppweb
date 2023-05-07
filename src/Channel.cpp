#include "Channel.hpp"
#include "EventLoop.hpp"
#include <assert.h>

namespace CPPWEB {

Channel::Channel(EventLoop* loop, int fd):
    polling(false),
    m_loop(loop),
    m_fd(fd),
    m_events(0), 
    m_revents(0),
    m_tied(false) {}

void Channel::update() {
    m_loop->updateChannel(this);
}

void Channel::handleEvents() {
    m_loop->assertInLoopThread();
    if (m_tied) {
        auto guard = m_tie.lock();
        if (guard != nullptr) {
            handleEventsWithGuard();
        }
    } else {
        handleEventsWithGuard();
    }
}

void Channel::tie(const std::shared_ptr<void>& obj) {
    m_tied = true;
    m_tie = obj;
}

void Channel::handleEventsWithGuard() {
    if ((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN)) {//对端关闭了套接字，会触发EPOLLHUP
        if (m_closeCallback) { m_closeCallback(); }
    }
    if (m_revents & EPOLLERR) {//EPOLLERR错误事件
        if (m_errorCallback) { m_errorCallback(); }
    }
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {//EPOLLPRI带外数据，对端关闭套接字或关闭写时，会触发EPOLLIN + EPOLLRDHUP事件
        if (m_readCallback) { m_readCallback(); }
    }
    if (m_revents & EPOLLOUT) {
        if (m_writeCallback) { m_writeCallback(); }
    }
}

}