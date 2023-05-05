#include "Channel.hpp"
#include "EventLoop.hpp"
#include <assert.h>

namespace CPPWEB {

Channel::Channel(EventLoop* loop, int fd):
    polling(false),
    m_loop(loop),
    m_fd(fd),
    m_events(0), 
    m_revents(0) {}

void Channel::update() {
    m_loop->updateChannel(this);
}

void Channel::handleEvents() {
    m_loop->assertInLoopThread();
    handleEventsWithGuard();
}

void Channel::handleEventsWithGuard() {
    if ((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN)) {//本端出错导致读写关闭触发EPOLLHUP
        if (m_closeCallback) { m_closeCallback(); }
    }
    if (m_revents & EPOLLERR) {//EPOLLERR错误事件
        if (m_errorCallback) { m_errorCallback(); }
    }
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {//EPOLLPRI带外数据，对端连接关闭时会触发EPOLLIN + EPOLLRDHUP事件
        if (m_readCallback) { m_readCallback(); }
    }
    if (m_revents & EPOLLOUT) {
        if (m_writeCallback) { m_writeCallback(); }
    }
}
}