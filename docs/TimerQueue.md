```cpp
class TimerQueue {
    typedef std::pair<TimePoint, Timer *> Entry;
    EventLoop* m_loop;
    const int m_timerFd;//文件描述符,用于接收定时发送的心跳包
    Channel m_timerChannel;
    std::set<Entry> m_timers;//按照Entry中的第一个元素，也就是TimePoint比大小
};
```

用红黑树结构std::set管理一组Timer<br>
std::set中TimePoint的最小的Timer的时间到达之后，出发handleRead函数，读取定时发送的心跳包，处理超时Timer（执行回调方法、restartTimer）
