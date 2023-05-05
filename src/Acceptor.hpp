#ifndef CPPWEB_ACCEPTOR_HPP
#define CPPWEB_ACCEPTOR_HPP

#include "nocopyable.hpp"
#include "InetAddress.hpp"
#include <functional>
#include "Channel.hpp"

namespace CPPWEB {

class EventLoop;
class Channel;

class Acceptor: public nocopyable {
public:
    typedef std::function<void(int sockfd, const InetAddress& local, const InetAddress& peer)> NewConnectioCallback;

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
    InetAddress m_localAddress;
    NewConnectioCallback m_newConnectionCallback;
    const int m_acceptFd;
    Channel m_acceptChannel;
};

}

#endif