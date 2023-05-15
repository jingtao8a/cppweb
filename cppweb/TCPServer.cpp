#include "TCPServer.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include "EventLoop.hpp"

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

TCPServer::TCPServer(EventLoop* loop, const InetAddress& local):
            m_loop(loop),
            m_threadNum(1),
            m_started(false),
            m_local(local),
            m_threadInitCallback(defaultThreadInitCallback),
            m_connectionCallback(defaultConnectionCallback),
            m_messageCallback(defaultMessageCallback),
            m_tcpServerSingle(loop, local) {
    INFO(logger, "create TCPServer() %s", m_local.toIpPort().c_str());
}

TCPServer::~TCPServer() {
    for (auto& loop : m_eventLoopList) {
        if (loop != nullptr) {
            loop->quit();
        }
    }
    for (auto& thread: m_threadPtrList) {
        thread->join();
    }
    TRACE(logger, "~TCPServer()");
}

void TCPServer::setThreadNum(size_t n) {
    m_loop->assertInLoopThread();
    assert(n > 0);
    assert(!m_started);
    m_threadNum = n;
    m_eventLoopList.resize(n);
}

void TCPServer::start() {
    if (m_started.exchange(true)) {
        return;
    }

    m_loop->runInLoop([this]() {this->startInLoop(); });
}

void TCPServer::startInLoop() {
    m_loop->assertInLoopThread();
    INFO(logger, "TCPServer::start() %s with %lu eventLoop thread(s)", m_local.toIpPort().c_str(), m_threadNum);
    m_tcpServerSingle.setConnectionCallback(m_connectionCallback);
    m_tcpServerSingle.setMessageCallback(m_messageCallback);
    m_tcpServerSingle.setWriteCompleteCallback(m_writeCompleteCallback);
    m_threadInitCallback(0);
    m_tcpServerSingle.start();

    for (size_t i = 1; i < m_threadNum; ++i) {
        auto thread = new std::thread(std::bind(&TCPServer::runInThread, this, i));
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_eventLoopList[i] == nullptr) {
                m_cond.wait(lock);
            }
        }
        m_threadPtrList.emplace_back(thread);
    }
}

void TCPServer::runInThread(size_t index) {
    EventLoop loop;
    TCPServerSingle server(&loop, m_local);
    server.setConnectionCallback(m_connectionCallback);
    server.setMessageCallback(m_messageCallback);
    server.setWriteCompleteCallback(m_writeCompleteCallback);
    
    m_eventLoopList[index] = &loop;
    m_cond.notify_one();

    m_threadInitCallback(index);
    server.start();
    loop.loop();
}

}