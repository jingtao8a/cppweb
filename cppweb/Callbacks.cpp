#include "Callbacks.hpp"
#include "Logger.hpp"
#include "Singleton.hpp"
#include "TCPConnection.hpp"
#include "Buffer.hpp"

namespace CPPWEB {

static auto& logger = Singleton<Logger>::GetInstance();

void defaultConnectionCallback(const TCPConnectionPtr& conn) {
    INFO(logger, "connection %s -> %s %s",
         conn->peer().toIpPort().c_str(),
         conn->local().toIpPort().c_str(),
         conn->isConnected() ? "up" : "down");
}

void defaultMessageCallback(const TCPConnectionPtr& conn, Buffer& buffer) {
    TRACE(logger, "connection %s -> %s recv %lu bytes",
          conn->peer().toIpPort().c_str(),
          conn->local().toIpPort().c_str(),
          buffer.readableBytes());
    buffer.retrieveAll();
}

void defaultThreadInitCallback(size_t index) {
    TRACE(logger, "EventLoop thread #%lu started", index);
}


}