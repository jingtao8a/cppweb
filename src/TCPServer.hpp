#ifndef CPPWEB_TCPSERVER_HPP
#define CPPWEB_TCPSERVER_HPP

#include "nocopyable.hpp"


namespace CPPWEB {

class EventLoop;
class InetAddress;

class TCPServer: public nocopyable {
public:
    TCPServer(EventLoop *loop, const InetAddress& local);
    ~TCPServer();

    void setNumThrea(size_t n);
    void setThreadInitCallback(const std::function<void(size_t n)>&  cb) { m_threadInitCallback = cb; }
    void setConnectionCallback(const )
private:
    std::function<void(size_t n)> m_threadInitCallback;
};

}


#endif