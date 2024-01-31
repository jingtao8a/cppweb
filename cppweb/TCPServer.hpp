#ifndef CPPWEB_TCPSERVER_HPP
#define CPPWEB_TCPSERVER_HPP

#include "nocopyable.hpp"
#include "TCPServerSingle.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>

namespace CPPWEB {

class EventLoop;
class InetAddress;

class TCPServer: public nocopyable {
public:
    TCPServer(EventLoop *loop, const InetAddress& local);
    ~TCPServer();

    void setThreadInitCallback(const ThreadInitCallback&  cb) { m_threadInitCallback = cb; }

    void setConnectionCallback(const ConnectionCallback& cb) { m_connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { m_writeCompleteCallback = cb; }
    
    void setThreadNum(size_t n);
    void start();
private:
    void startInLoop();
    void runInThread(size_t index);

private:
    typedef std::unique_ptr<std::thread> ThreadPtr;

    ThreadInitCallback m_threadInitCallback;//线程初始化函数
    ConnectionCallback m_connectionCallback;//连接建立和关闭回调函数
    MessageCallback m_messageCallback;//收到消息的回调函数
    WriteCompleteCallback m_writeCompleteCallback;//发送消息后的回调函数

    EventLoop *m_loop;
    TCPServerSingle m_tcpServerSingle;
    
    std::vector<ThreadPtr> m_threadPtrList;
    std::vector<EventLoop*> m_eventLoopList;
    std::mutex m_mutex;
    std::condition_variable m_cond;

    std::atomic_bool m_started;
    size_t m_threadNum;
    InetAddress m_local;
};

}


#endif