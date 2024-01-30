```cpp
class Timer {
    std::function<void()> m_callback; //回调函数
    TimePoint m_when; //触发时间节点
    const Nanoseconds m_interval; //时间间隔
    bool m_repeat; //是否可重复
    bool m_canceled; //是否已经cancel
};
```
