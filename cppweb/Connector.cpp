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
    m_channel(loop, m_sockfd) {

    m_channel.setWriteCallback([this]() {this->handleWrite(); });
}

Connector::~Connector() {
    ::close(m_sockfd);
}

void Connector::start() {//发起连接事件
    m_loop->assertInLoopThread();

    int ret = ::connect(m_sockfd, m_peer.getSockaddr(), m_peer.getSocklen());
    if (ret == -1 && errno == EINPROGRESS) {
        m_channel.enableWrite();//正在连接过程中,等待连接完成后再触发handleWrite事件
    } else if (ret == 0){
        handleWrite();//连接成功
    } else {//连接失败
        SYSERR(logger, "Connector::connect()");
    }
}

void Connector::handleWrite() {
    m_loop->assertInLoopThread();
    if (!m_channel.isNoneEvents()) {
        m_channel.disableAll();//取消所有事件
    }
    int err;
    socklen_t len = sizeof(err);
    int ret = ::getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (ret == 0) {
        errno = err;
    }
    if (errno != 0) {//连接失败
        SYSERR(logger, "Connector::connect()");
    } else {//连接成功
        struct sockaddr_in addr;
        len = sizeof(addr);
        ret = ::getsockname(m_sockfd, reinterpret_cast<sockaddr*>(&addr), &len);
        if (ret == -1) {
            SYSERR(logger, "Connection::getsockname()");
        }
        InetAddress local;
        local.setAddress(addr);
        
        m_newConnectionCallback(m_sockfd, local, m_peer);//新建连接TCPConnection
    }
}
}