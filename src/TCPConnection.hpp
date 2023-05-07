#ifndef CPPWEB_TCPCONNECTION_HPP
#define CPPWEB_TCPCONNECTION_HPP

#include "nocopyable.hpp"
#include "InetAddress.hpp"
#include "Channel.hpp"
#include "Buffer.hpp"
#include <stddef.h>
#include <memory>
#include "Callbacks.hpp"

namespace CPPWEB {

enum ConnectionState {
    kConnecting,
    kConnected,
    kDisconnecting,
    kDisconnected,
};


class TCPConnection: public nocopyable, public std::enable_shared_from_this<TCPConnection> {

public:
    TCPConnection(EventLoop* loop, int sockfd, const InetAddress& local, const InetAddress& peer);
    ~TCPConnection();

    void setMessageCallback(const MessageCallback& cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { m_writeCompleteCallback = cb; }
    void setCloseCallback(const CloseCallback& cb) { m_closeCallback = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t mark) 
    { m_highWaterMarkCallback = cb; m_highWaterMark = mark; }

    void connectEstablished();

    bool isConnected() const { return m_state == kConnected; }
    bool isDisconnected() const { return m_state == kDisconnected; }

    const InetAddress& local() const { return m_local; }
    const InetAddress& peer() const { return m_peer; }
    const Buffer& inputBuffer() const { return m_inputBuffer; }
    const Buffer& outputBuffer() const { return m_outputBuffer; }
    std::string name() const { return m_peer.toIpPort() + " -> " + m_local.toIpPort(); }

    // I/O operations
    void send(const std::string& data);
    void send(const char* data, size_t len);
    void send(Buffer& buffer);
    void shutdown();
    void forceClose();

    void stopRead();
    void startRead();
    bool isReading() { return m_channel.isReading(); }
private:
    void sendInLoop(const std::string& message);
    void sendInLoop(const char* data, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    int stateAtomicGetAndSet(int newState);

private:
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    CloseCallback m_closeCallback;
    HighWaterMarkCallback m_highWaterMarkCallback;
    size_t m_highWaterMark;


    EventLoop* m_loop;
    const int m_sockfd;
    Channel m_channel;
    int m_state;
    InetAddress m_local;
    InetAddress m_peer;
    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
};


}

#endif