#include "../src/Singleton.hpp"
#include "../src/Logger.hpp"
#include "../src/EventLoop.hpp"
#include "../src/TCPServerSingle.hpp"
#include "../src/Buffer.hpp"
#include "../src/TCPConnection.hpp"

static auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

class DiscardServer {
public:
    DiscardServer(CPPWEB::EventLoop* loop, const CPPWEB::InetAddress& addr):
        m_server(loop, addr) {
        m_server.setMessageCallback(std::bind(&DiscardServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    }

    void start() {
        m_server.start();
    }

    void onMessage(const CPPWEB::TCPConnectionPtr& conn, CPPWEB::Buffer& buffer) {
        TRACE(logger, "connection %s recv %lu bytes",
              conn->name().c_str(),
              buffer.readableBytes());
        std::string str = buffer.retrieveAllAsString();
        TRACE(logger, "%s", str.c_str());
        conn->send(str);
    }
private:
    CPPWEB::TCPServerSingle m_server;
};


int main() {
    CPPWEB::EventLoop loop;
    CPPWEB::InetAddress addr(9999);
    DiscardServer server(&loop, addr);
    server.start();
    loop.loop();
    return 0;
}