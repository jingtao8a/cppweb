#ifndef CPPWEB_ACCEPTOR_HPP
#define CPPWEB_ACCEPTOR_HPP

#include "nocopyable.hpp"
#include "InetAddress.hpp"
#include "Channel.hpp"
#include "Callbacks.hpp"

namespace CPPWEB {

class EventLoop;

class Acceptor: public nocopyable {
public:
    Acceptor(EventLoop* loop, const InetAddress& localAddress);
    ~Acceptor();
    bool isListening() const { return m_listening; }

    void listen();
    void setNewConnectionCallback(const NewConnectioCallback& cb) { m_newConnectionCallback = cb; }

private:
    void handleRead();
private:
    bool m_listening;

    EventLoop *m_loop;
    const InetAddress m_localAddress;
    const int m_acceptFd;
    Channel m_acceptChannel;
    
    NewConnectioCallback m_newConnectionCallback;
};

}

#endif