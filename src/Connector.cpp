#include "Connector.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include "EventLoop.hpp"
#include <assert.h>
#include <unistd.h>

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

int createSocket();//声明

Connector::Connector(EventLoop* loop, const InetAddress& peer):
    m_loop(loop),
    m_peer(peer),
    m_sockfd(createSocket()),
    m_connected(false),
    m_started(false),
    m_channel(loop, m_sockfd) {

    m_channel.setWriteCallback([this]() {this->handleWrite(); });
}

Connector::~Connector() {
    if (!m_connected) {
        ::close(m_sockfd);
    }
}

void Connector::start() {//发起连接事件
    m_loop->assertInLoopThread();
    assert(!m_started);
    m_started = true;

    int ret = ::connect(m_sockfd, m_peer.getSockaddr(), m_peer.getSocklen());
    if (ret == -1) {
        if (errno != EINPROGRESS) {
            handleWrite();
        } else {
            m_channel.enableWrite();//连接失败，等待下次发起连接
        }
    } else {
        handleWrite();
    }
}

void Connector::handleWrite() {
    m_loop->assertInLoopThread();
    assert(m_started);

    m_channel.disableAll();//连接成功，取消事件
    
    int err;
    socklen_t len = sizeof(err);
    int ret = ::getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (ret == 0) {
        errno = err;
    }
    if (errno != 0) {//出错
        SYSERR(logger, "Connector::connect()");
        if (m_errorCallback) {
            m_errorCallback();
        }
    } else {//未出错
        struct sockaddr_in addr;
        len = sizeof(addr);
        void *any = &addr;
        ret = ::getsockname(m_sockfd, static_cast<sockaddr*>(any), &len);
        if (ret == -1) {
            SYSERR(logger, "Connection::getsockname()");
        }
        InetAddress local;
        local.setAddress(addr);

        m_connected = true;
        m_newConnectionCallback(m_sockfd, local, m_peer);//新建连接TCPConnection
    }
}
}