#ifndef CPPWEB_CALLBACKS_HPP
#define CPPWEB_CALLBACKS_HPP
#include <functional>
#include <memory>

namespace CPPWEB {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

class TCPConnection;
typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;
class Buffer;
class InetAddress;

//TCPConnection
typedef std::function<void(const TCPConnectionPtr&, Buffer&)> MessageCallback;
typedef std::function<void(const TCPConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TCPConnectionPtr&)> CloseCallback;
typedef std::function<void(const TCPConnectionPtr&, size_t)> HighWaterMarkCallback;

//Acceptor || Connector
typedef std::function<void(int sockfd, const InetAddress& local, const InetAddress& peer)> NewConnectioCallback;

//Connector
typedef std::function<void()> ErrorCallback;

//TCPServerSingle
typedef std::function<void(const TCPConnectionPtr&)> ConnectionCallback;

//ThreadPool TCPServer
typedef std::function<void(size_t)> ThreadInitCallback;

void defaultConnectionCallback(const TCPConnectionPtr& conn);
void defaultMessageCallback(const TCPConnectionPtr& conn, Buffer& buffer);
void defaultThreadInitCallback(size_t index);

}


#endif