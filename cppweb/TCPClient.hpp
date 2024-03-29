#ifndef CPPWEB_TCPCLIENT_HPP
#define CPPWEB_TCPCLIENT_HPP

#include "Connector.hpp"
#include "TCPConnection.hpp"
#include "Timer.hpp"
#include "Callbacks.hpp"

namespace CPPWEB {
class TCPClient: public nocopyable {
public:
    TCPClient(EventLoop* loop, const InetAddress& peer);
    ~TCPClient();

    void start();
    void setConnectionCallback(const ConnectionCallback& cb) { m_connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { m_writeCompleteCallback = cb; }

private:
    void retry();
    void newConnection(int connfd, const InetAddress& local, const InetAddress& peer);
    void closeConnection(const TCPConnectionPtr& conn);

private:
    bool m_connected;
    const InetAddress m_peer;
    Timer* m_retryTimer;

    MessageCallback m_messageCallback;//收到数据的回调函数
    WriteCompleteCallback m_writeCompleteCallback;//数据发送完毕的回调函数
    ConnectionCallback m_connectionCallback;//连接建立和关闭后的回调函数

    TCPConnectionPtr m_connection;
    EventLoop* m_loop;
    Connector m_connector;
};

}

#endif