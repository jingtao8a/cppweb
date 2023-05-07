#include "Acceptor.hpp"
#include <unistd.h>
#include "Logger.hpp"
#include "Singleton.hpp"
#include "EventLoop.hpp"

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

int createSocket() {
    int ret = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (ret == -1) {
        SYSFATAL(logger, "Acceptor::socket() || Connector::socket()");
    }
    return ret;
}

Acceptor::Acceptor(EventLoop* loop, const InetAddress& localAddress) :
    m_loop(loop),
    m_localAddress(localAddress),
    m_acceptFd(createSocket()),
    m_acceptChannel(loop, m_acceptFd),
    m_listening(false) {
    
    int on = 1;
    int ret = ::setsockopt(m_acceptFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));//用于对TCP套接字处于TIME_WAIT状态下的socket，才可以重复绑定使用
    if (ret == -1) {
        SYSFATAL(logger, "Acceptor::setsockopt() SO_REUSEADDR");
    }
    ret = ::setsockopt(m_acceptFd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));//允许多个socket绑定到同一个ip+port
    if (ret == -1) {
        SYSFATAL(logger, "Acceptor::setsockopt() SO_REUSEPORT");
    }
    ret = ::bind(m_acceptFd, m_localAddress.getSockaddr(), m_localAddress.getSocklen());
    if (ret == -1) {
        SYSFATAL(logger, "Acceptor::bind()");
    }
}

Acceptor::~Acceptor() {
    ::close(m_acceptFd);
}

void Acceptor::listen() {
    m_loop->assertInLoopThread();
    int ret = ::listen(m_acceptFd, SOMAXCONN);
    if (ret == -1) {
        SYSFATAL(logger, "Acceptor::listen()");
    }
    m_acceptChannel.setReadCallback([this]() { this->handleRead(); });
    m_acceptChannel.enableRead();
}

void Acceptor::handleRead() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    int sockfd = ::accept4(m_acceptFd, reinterpret_cast<sockaddr*>(&addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    
    if (sockfd == -1) {
        int savedErrno = errno;
        SYSERR(logger, "Acceptor::accept4()");
        switch (savedErrno) {
            case ECONNABORTED:
            case EMFILE:
                break;
            default:
                FATAL(logger, "unexpected accept4() error");    
        }
    }

    if (m_newConnectionCallback) {
        InetAddress peer;
        peer.setAddress(addr);
        m_newConnectionCallback(sockfd, m_localAddress, peer);
    } else {
        ::close(sockfd);
    }
}



}