#ifndef CPPWEB_TCPSERVERSINGLE_HPP 
#define CPPWEB_TCPSERVERSINGLE_HPP

#include <unordered_set>
#include "nocopyable.hpp"
#include "Acceptor.hpp"

namespace CPPWEB {

class TCPServerSingle: public nocopyable {
public:
    TCPServerSingle(EventLoop* loop, const InetAddress& local);

    void setConnectionCallback(const std::function<void(const TCPConnectionPtr&)>& cb) { m_connectionCallback = cb; }//自己使用的回调函数
    void setMessageCallback(const std::function<void(const TCPConnectionPtr&, Buffer&)>& cb) { m_messageCallback = cb; }//TCPconnection使用的回调函数
    void setWriteCompleteCallback(const std::function<void(const TCPConnectionPtr&)>& cb) { m_writeCompleteCallback = cb; }//TCPConnection使用的回调函数

    void start();

private:
    void newConnetction(int connfd, const InetAddress& local, const InetAddress& peer);//作为Acceptor的回调函数
    void closeConnection(const TCPConnectionPtr& conn);//作为TCPConnection的回调函数

private:
    MessageCallback m_messageCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    ConnectionCallback m_connectionCallback;

    std::unordered_set<TCPConnectionPtr> m_connectionSet;
    EventLoop* m_loop;
    Acceptor m_acceptor;
};



}

#endif