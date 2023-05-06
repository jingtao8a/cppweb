#ifndef CPPWEB_TCPSERVERSINGLE_HPP 
#define CPPWEB_TCPSERVERSINGLE_HPP

#include "nocopyable.hpp"
#include <functional>
#include <unordered_set>
#include "Acceptor.hpp"
#include "TCPConnection.hpp"

namespace CPPWEB {

class TCPConnection;
class Buffer;


class TCPServerSingle: public nocopyable {
public:
    TCPServerSingle(EventLoop* loop, const InetAddress& local);

    void setConnectionCallback(const std::function<void(const TCPConnectionPtr&)>& cb) { m_connectionCallback = cb; }
    void setMessageCallback(const std::function<void(const TCPConnectionPtr&, Buffer&)>& cb) { m_messageCallback = cb; }
    void setWriteCompleteCallback(const std::function<void(const TCPConnectionPtr&)>& cb) { m_writeCompleteCallback = cb; }

    void start();

private:
    void newConnetction(int connfd, const InetAddress& local, const InetAddress& peer);
    void closeConnection(const TCPConnectionPtr& conn);
private:
    std::function<void(const TCPConnectionPtr&)> m_connectionCallback;
    std::function<void(const TCPConnectionPtr&, Buffer&)> m_messageCallback;
    std::function<void(const TCPConnectionPtr&)> m_writeCompleteCallback;

    std::unordered_set<TCPConnectionPtr> m_connectionSet;
    EventLoop* m_loop;
    Acceptor m_acceptor;

};

}

#endif