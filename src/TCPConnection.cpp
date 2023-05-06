#include "TCPConnection.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include <unistd.h>
#include <assert.h>

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

enum ConnectionState {
    kConnecting,
    kConnected,
    kDisconnecting,
    kDisconnected,
};

TCPConnection::TCPConnection(EventLoop* loop, int sockfd, const InetAddress& local, const InetAddress& peer): 
    m_loop(loop),
    m_sockfd(sockfd),
    m_channel(loop, sockfd),
    m_state(kConnecting),
    m_local(local),
    m_peer(peer),
    m_highWaterMark(0) {

    m_channel.setReadCallback([this](){ this->handleRead(); });
    m_channel.setWriteCallback([this](){ this->handleWrite(); });
    m_channel.setCloseCallback([this](){ this->handleClose(); });
    m_channel.setErrorCallback([this](){ this->handleError(); });

    TRACE(logger, "TCPConnection() %s fd=%d", name().c_str(), m_sockfd);
}

TCPConnection::~TCPConnection() {
    assert(m_state == kDisconnected);
    ::close(m_sockfd);
    TRACE(logger, "~TCPConnection() %s fd=%d", name().c_str(), m_sockfd);
}

void TCPConnection::connectEstablished() {
    assert(m_state == kConnecting);
    m_state = kConnected;
    m_channel.enableRead();
}

void TCPConnection::handleRead() {
}
    
void TCPConnection::handleWrite() {

}

void TCPConnection::handleClose() {
    m_loop->assertInLoopThread();
    assert(m_state ==  kConnected || m_state == kDisconnecting );
    m_state = kDisconnected;
    m_channel.disableAll();
    m_closeCallback(shared_from_this());
}

void TCPConnection::handleError() {
    int err;
    socklen_t len = sizeof(err);
    int ret = getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (ret != -1) {
        errno = err;
    }
    SYSERR(logger, "TCPConnection::handleError()");
}

}