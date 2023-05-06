#include "TCPServerSingle.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"


namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

TCPServerSingle::TCPServerSingle(EventLoop* loop, const InetAddress& local):
    m_loop(loop),
    m_acceptor(loop, local) {
    
    m_acceptor.setNewConnectionCallback(std::bind(&TCPServerSingle::newConnetction, this, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TCPServerSingle::start() {
    m_acceptor.listen();
}

void TCPServerSingle::newConnetction(int connfd, const InetAddress& local, const InetAddress& peer) {
    m_loop->assertInLoopThread();
    
}

}