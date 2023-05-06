#ifndef CPPWEB_TCPCONNECTION_HPP
#define CPPWEB_TCPCONNECTION_HPP

#include "nocopyable.hpp"
#include "InetAddress.hpp"
#include "Channel.hpp"
#include "Buffer.hpp"
#include <stddef.h>
#include <memory>

namespace CPPWEB {

class EventLoop;

class TCPConnection;
typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

class TCPConnection: public nocopyable, std::enable_shared_from_this<TCPConnection>{

public:
    TCPConnection(EventLoop* loop, int sockfd, const InetAddress& local, const InetAddress& peer);
    ~TCPConnection();

    void connectEstablished();

    void setMessageCallback(const std::function<void(const TCPConnectionPtr&, Buffer&)>& cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const std::function<void(const TCPConnectionPtr&)>& cb) { m_writeCompleteCallback = cb; }
    void setHighWaterMarkCallback(const std::function<void(const TCPConnectionPtr&, size_t)>& cb, size_t mark) 
    { m_highWaterMarkCallback = cb; m_highWaterMark = mark; }

    void setCloseCallback(const std::function<void(const TCPConnectionPtr&)>& cb) { m_closeCallback = cb; }


    std::string name() const { return m_peer.toIpPort() + " -> " + m_local.toIpPort(); }

private:
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

private:
    std::function<void(const TCPConnectionPtr&, Buffer&)> m_messageCallback;
    std::function<void(const TCPConnectionPtr&)> m_writeCompleteCallback;
    std::function<void(const TCPConnectionPtr&, size_t)> m_highWaterMarkCallback;
    size_t m_highWaterMark;

    std::function<void(const TCPConnectionPtr&)> m_closeCallback;

    EventLoop* m_loop;
    const int m_sockfd;
    Channel m_channel;
    int m_state;
    InetAddress m_local;
    InetAddress m_peer;
    Buffer m_inputBuffer;
    Buffer m_ouputBuffer;
    
};

}

#endif