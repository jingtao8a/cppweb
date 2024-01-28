add_library(cppweb STATIC
    Logger.cpp
    InetAddress.cpp
    ThreadPool.cpp
    TimerQueue.cpp
    Channel.cpp
    Epoller.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    Buffer.cpp
    TCPConnection.cpp
    TCPServerSingle.cpp
    Acceptor.cpp
    Callbacks.cpp
    TCPClient.cpp
    Connector.cpp
    TCPServer.cpp)
target_link_libraries(cppweb pthread)
install(TARGETS cppweb DESTINATION lib)

set(HEADERS
    Acceptor.hpp
    Buffer.hpp
    Callbacks.hpp
    Channel.hpp
    Connector.hpp
    Epoller.hpp
    EventLoop.hpp
    EventLoopThread.hpp
    InetAddress.hpp
    Logger.hpp
    nocopyable.hpp
    Singleton.hpp
    TCPClient.hpp
    TCPConnection.hpp
    TCPServer.hpp
    TCPServerSingle.hpp
    ThreadPool.hpp
    Timer.hpp
    TimerQueue.hpp
    Timestamp.hpp)

install(FILES ${HEADERS} DESTINATION include/cppweb)

