> Channel对socket套接字进行了一层封装
> 包含读、写、关闭、错误的回调函数
> EventLoop使用Epoller进行poll轮询出激活的channel，并调用handleEvents


```cpp
class Channel {
private:
    std::function<void()> m_readCallback;//读事件
    std::function<void()> m_writeCallback;//写事件
    std::function<void()> m_closeCallback;//关闭事件
    std::function<void()> m_errorCallback;//错误事件
    
    int m_fd;//套接字
    unsigned m_events; // 待监听事件
    unsigned m_revents; // 触发事件
    EventLoop *loop;
    bool polling;//标志该Channel是否在轮询
};
```

```cpp
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
```