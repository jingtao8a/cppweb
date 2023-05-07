#include "TCPClient.hpp"
#include "TCPServerSingle.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

TCPClient::TCPClient(EventLoop* loop, const InetAddress& peer):
    m_loop(loop),
    m_peer(peer),
    m_connected(false),
    m_retryTimer(nullptr),
    m_connector(new Connector(loop, peer)),
    m_connectionCallback(defaultConnectionCallback),
    m_messageCallback(defaultMessageCallback) {

    m_connector->setNewConnectionCallback(std::bind(&TCPClient::newConnection, this, _1, _2, _3));
}

TCPClient::~TCPClient() {
    if (m_connection && !m_connection->isDisconnected()) {
        m_connection->forceClose();
    }
    if (m_retryTimer != nullptr) {
        m_loop->cancelTimer(m_retryTimer);
    }
}

void TCPClient::start() {
    m_loop->assertInLoopThread();
    m_connector->start();
    m_retryTimer = m_loop->runEvery(Nanoseconds(3000000000), [this]() {this->retry();});
}

void TCPClient::retry() {
    m_loop->assertInLoopThread();
    if (m_connected) {
        return;
    }

    WARN(logger, "TCPClient::retry() reconnect %s...", m_peer.toIpPort().c_str());
    m_connector = std::make_unique<Connector>(m_loop, m_peer);
    m_connector->setNewConnectionCallback(std::bind(&TCPClient::newConnection, this, _1, _2, _3));
    m_connector->start();
}

void TCPClient::newConnection(int connfd, const InetAddress& local, const InetAddress& peer) {
    m_loop->assertInLoopThread();
    m_loop->cancelTimer(m_retryTimer);
    m_retryTimer = nullptr;
    m_connected = true;
    m_connection = std::make_shared<TCPConnection>(m_loop, connfd, local, peer);
    m_connection->setMessageCallback(m_messageCallback);
    m_connection->setWriteCompleteCallback(m_writeCompleteCallback);
    m_connection->setCloseCallback(std::bind(&TCPClient::closeConnection, this, std::placeholders::_1));
    m_connection->connectEstablished();
    if (m_connectionCallback) {
        m_connectionCallback(m_connection);
    }
}

void TCPClient::closeConnection(const TCPConnectionPtr& conn) {
    m_loop->assertInLoopThread();
    assert(m_connection != nullptr);
    m_connection.reset();
    m_connectionCallback(conn);
}

}