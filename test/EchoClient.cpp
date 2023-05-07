#include "../src/Logger.hpp"
#include "../src/Singleton.hpp"
#include "../src/EventLoop.hpp"
#include "../src/TCPClient.hpp"
#include "../src/nocopyable.hpp"
#include <iostream>
#include <thread>

static auto& logger = CPPWEB::Singleton<CPPWEB::Logger>::GetInstance();

class UserInput: public CPPWEB::nocopyable {
public:
    UserInput(const CPPWEB::TCPConnectionPtr& conn):
        m_conn(conn) {
        m_conn->setMessageCallback(std::bind(&UserInput::onMessage, this, CPPWEB::_1, CPPWEB::_2));
    }

    void onMessage(const CPPWEB::TCPConnectionPtr& conn, CPPWEB::Buffer& buffer) {
        std::cout << buffer.retrieveAllAsString() << std::endl;
    }

    void run() {
        std::string line;
        while(std::getline(std::cin, line)) {
            m_conn->send(line);
        }
        m_conn->shutdown();
    }
private:
    CPPWEB::TCPConnectionPtr m_conn;
};

class EchoBench: public CPPWEB::nocopyable {
public:
    EchoBench(CPPWEB::EventLoop* loop, const CPPWEB::InetAddress& addr) :
        m_loop(loop),
        m_client(loop, addr) {
        
        m_client.setConnectionCallback(std::bind(&EchoBench::onConnection, this, CPPWEB::_1));
    }

    void start() {
        m_client.start();
    }
    void onConnection(const CPPWEB::TCPConnectionPtr& conn) {
        INFO(logger, "connection %s is [%s]", conn->name().c_str(), conn->isConnected() ? "up" : "down");
        auto th = std::thread([conn]() {
            UserInput user(conn);
            user.run();
        });
        th.detach();
    }
private:
    CPPWEB::EventLoop *m_loop;
    CPPWEB::TCPClient m_client;
};

int main() {
    CPPWEB::EventLoop loop;
    CPPWEB::InetAddress addr("192.168.159.130", 9999);
    EchoBench client(&loop, addr);
    client.start();
    loop.loop();
    return 0;
}
