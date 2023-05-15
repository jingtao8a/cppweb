#include "TCPServerSingle.hpp"
#include "EventLoop.hpp"
#include "TCPConnection.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"


namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();


TCPServerSingle::TCPServerSingle(EventLoop* loop, const InetAddress& local):
    m_loop(loop),
    m_acceptor(loop, local) {
    
    m_acceptor.setNewConnectionCallback(std::bind(&TCPServerSingle::newConnetction, this, _1, _2, _3));
}

void TCPServerSingle::start() {
    m_acceptor.listen();
}

void TCPServerSingle::newConnetction(int connfd, const InetAddress& local, const InetAddress& peer) {
    m_loop->assertInLoopThread();
    auto conn = std::make_shared<TCPConnection>(m_loop, connfd, local, peer);
    m_connectionSet.insert(conn);
    conn->setMessageCallback(m_messageCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TCPServerSingle::closeConnection, this, _1));
    conn->connectEstablished();
    if (m_connectionCallback) {
        m_connectionCallback(conn);
    }
}

void TCPServerSingle::closeConnection(const TCPConnectionPtr& conn) {
    m_loop->assertInLoopThread();
    m_connectionSet.erase(conn);//移除连接
    if (m_connectionCallback) {
        m_connectionCallback(conn);
    }
}

}