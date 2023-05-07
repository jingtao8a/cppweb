#include "TCPConnection.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include <unistd.h>
#include <assert.h>

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();


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
    ::close(m_sockfd);//关闭套接字
    TRACE(logger, "~TCPConnection() %s fd=%d", name().c_str(), m_sockfd);
}

void TCPConnection::connectEstablished() {
    m_loop->assertInLoopThread();
    assert(m_state == kConnecting);
    m_state = kConnected;
    m_channel.enableRead();
}

void TCPConnection::send(Buffer& buffer) {
    std::string data = buffer.retrieveAllAsString();
    send(data);
}

void TCPConnection::send(const std::string& data) { 
    send(data.data(), data.length()); 
}

void TCPConnection::send(const char* data, size_t len) {
    if (m_state != kConnected) {
        WARN(logger, "TCPConnection::send() not connected, give up send");
        return;
    }
    m_loop->runInLoop([ptr = shared_from_this(), str=std::string(data, data + len)]() { ptr->sendInLoop(str); });
}


void TCPConnection::sendInLoop(const std::string& message) {
    sendInLoop(message.data(), message.size());
}

void TCPConnection::sendInLoop(const char* data, size_t len) {
    if (m_state == kDisconnected) {
        WARN(logger, "TCPConnection::sendInLoop() disconnected, give up read");
    }
    ssize_t n = 0;
    size_t remain = len;
    bool falutError = false;
    if (!m_channel.isWriting()) {
        //首次进行发送
        assert(m_outputBuffer.readableBytes() == 0);
        n = ::write(m_sockfd, data, len);//注意m_sockfd为非阻塞模式，如果TCP写缓冲区满会直接返回0或-1 EAGAIN
        if (n == -1) {
            if (errno != EAGAIN) {
                SYSERR(logger, "TCPConnection::write()");
                if (errno == EPIPE || errno == ECONNRESET) {
                    falutError = true;//对方连接已经关闭
                }
            }
            n = 0;
        } else {
            remain -= static_cast<size_t>(n);
            if (remain == 0 && m_writeCompleteCallback) {//发送完成
                m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
            }
        }
    }

    if (!falutError && remain > 0) {//还未将数据发完，且连接正常
        if (m_highWaterMarkCallback) {
            size_t oldLen = m_outputBuffer.readableBytes();
            size_t newLen = oldLen + remain;
            if (oldLen < m_highWaterMark && newLen >= m_highWaterMark) {
                m_loop->queueInLoop(std::bind(m_highWaterMarkCallback, shared_from_this(), newLen));
            }
        }
        m_outputBuffer.append(data + n, remain);//放入发送缓存
        m_channel.enableWrite();//等待写事件
    }
}

void TCPConnection::shutdown() {
    assert(m_state <= kDisconnecting);
    if (stateAtomicGetAndSet(kDisconnecting) == kConnected) {
        m_loop->runInLoop(std::bind(&TCPConnection::shutdownInLoop, shared_from_this()));
    }
}

void TCPConnection::shutdownInLoop() {
    m_loop->assertInLoopThread();
    if (m_state != kDisconnected && !m_channel.isWriting()) {
        if (::shutdown(m_sockfd, SHUT_WR) == -1) {//关闭写，发送FIN
            SYSERR(logger, "TCPConnection::shutdown()");
        }
    }
}

void TCPConnection::forceClose() {
    if (m_state != kDisconnected) {
        if (stateAtomicGetAndSet(kDisconnecting) != kDisconnected) {
            m_loop->queueInLoop(std::bind(&TCPConnection::forceCloseInLoop, shared_from_this()));
        }
    }
}

void TCPConnection::forceCloseInLoop() {
    m_loop->assertInLoopThread();
    if (m_state != kDisconnected) {
        handleClose();
    }
}

void TCPConnection::stopRead() {
    m_loop->runInLoop([this]() {
        if (this->m_channel.isReading()) {
            this->m_channel.disableRead();
        }
    });
}

void TCPConnection::startRead() {
    m_loop->runInLoop([this](){
        if (!this->m_channel.isReading()) {
            this->m_channel.enableRead();
        }
    });
}

void TCPConnection::handleRead() {
    m_loop->assertInLoopThread();
    assert(m_state != kDisconnected);
    int savedErrno;
    ssize_t n = m_inputBuffer.readFd(m_sockfd, &savedErrno);
    if (n == -1) {//因为是由epoll_wait唤醒，说明接收缓存是有空间，正常情况都可以读出数据
        errno = savedErrno;
        SYSERR(logger, "TCPConnection::read()");
        handleError();
    } else if (n == 0) {//没有读出数据，对方可能关闭了写或者关闭了套接字
        handleClose();//直接做关闭操作
    } else {
        if (m_messageCallback) {
            m_messageCallback(shared_from_this(), m_inputBuffer);
        }
    }
}

void TCPConnection::handleWrite() {
    if (m_state == kDisconnected) {
        WARN(logger, "TCPConnection::handleWrite() disconnected, give up writing %lu bytes", m_outputBuffer.readableBytes());
        return;
    }

    assert(m_outputBuffer.readableBytes() > 0);
    assert(m_channel.isWriting());
    ssize_t n = ::write(m_sockfd, m_outputBuffer.peek(), m_outputBuffer.readableBytes());
    if (n == -1) {//因为是由epoll_wait唤醒，说明发送缓存是有空间，正常情况都可以发送出数据
        SYSERR(logger, "TCPConnection::write()");
    } else {
        m_outputBuffer.retrieve(static_cast<size_t>(n));
        if (m_outputBuffer.readableBytes() == 0) {//已经发送完毕了
            m_channel.disableWrite();//取消写事件
            if (m_state == kDisconnecting) {
                shutdownInLoop();//关闭写操作
            }
            if (m_writeCompleteCallback) {
                m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
            }
        }
    }
}

void TCPConnection::handleClose() {//对方关闭了套接字
    m_loop->assertInLoopThread();
    assert(m_state ==  kConnected || m_state == kDisconnecting );
    m_state = kDisconnected;
    m_channel.disableAll();
    if (m_closeCallback) {
        m_closeCallback(shared_from_this());
    }
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


int TCPConnection::stateAtomicGetAndSet(int newState) {
    return __atomic_exchange_n(&m_state, newState, __ATOMIC_SEQ_CST);
}

}