#ifndef CPPWEB_CONNECTOR_HPP
#define CPPWEB_CONNECTOR_HPP

#include "nocopyable.hpp"
#include "InetAddress.hpp"
#include "Channel.hpp"
#include "Callbacks.hpp"

namespace CPPWEB {

class Connector: public nocopyable {
public:
    Connector(EventLoop* loop, const InetAddress& peer);
    ~Connector();

    void start();

    void setNewConnectionCallback(const NewConnectioCallback& cb) { m_newConnectionCallback = cb; }

    void setErrorCallback(const std::function<void()>& cb) { m_errorCallback = cb; }

private:
    void handleWrite();

private:
    bool m_connected;
    bool m_started;

    EventLoop* m_loop;
    const InetAddress m_peer;
    const int m_sockfd;
    Channel m_channel;
    
    NewConnectioCallback m_newConnectionCallback;
    std::function<void()> m_errorCallback;
};

}

#endif