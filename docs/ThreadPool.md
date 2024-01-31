#### 生产者-消费者模型

消费者为线程池中的线程

```cpp
class ThreadPool {
public:
    ThreadPool(size_t numThread, size_t maxQueueSize, const std::function<void(size_t index)>& cb) :
        m_maxQueueSize(maxQueueSize), 
        m_threadInitCallback(cb),
        m_running(true) {
        for (size_t i = 1; i <= numThread; ++i) {//numThread为线程数量
            m_threadList.emplace_back(new std::thread([this, i]() { this->runInThread(i); }));//初始化线程池
        }
    }

    void runInThread(size_t index) {
        if (m_threadInitCallback) {
            m_threadInitCallback(index);
        }

        while (m_running) {
            if (auto task = takeTask()) {
                task();
            }
        }
    }

    // 生产者
    void runTask(const std::function<void()>& task) {
        assert(m_running);
        if (m_threadList.empty()) {
            task();
        } else {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_taskQueue.size() >= m_maxQueueSize) {//任务队列已满
                m_notFull.wait(lock);//m_notFull释放锁
            }
            m_taskQueue.push_back(task);
            m_notEmpty.notify_one();//m_notEmpty唤醒
        }
    }

    // 消费者
    std::function<void()> takeTask() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_taskQueue.empty() && m_running) {//任务队列为空
            m_notEmpty.wait(lock);//m_notEmpty释放锁，阻塞等待notify唤醒
        }
        std::function<void()> task;
        if (!m_taskQueue.empty()) {
            task = m_taskQueue.front();//拿任务task
            m_taskQueue.pop_front();
            m_notFull.notify_one();//m_notFull唤醒
        }
        return task;
    }


private:
    std::vector<std::unique_ptr<std::thread> > m_threadList;//线程对象池列表
    
    std::deque<std::function<void()> > m_taskQueue;//任务队列
    const size_t m_maxQueueSize;//任务队列中最大的大小
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;
    std::mutex m_mutex;

    std::function<void(size_t index)> m_threadInitCallback;//初始化线程的回调函数
    std::atomic_bool m_running;
};
```
