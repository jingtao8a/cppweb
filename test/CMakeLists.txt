add_executable(test test.cpp)
target_link_libraries(test cppweb)

add_executable(testlogger testlogger.cpp)
target_link_libraries(testlogger cppweb)

add_executable(testInetAddress testInetAddress.cpp)
target_link_libraries(testInetAddress cppweb)

add_executable(testthreadpool testthreadpool.cpp)
target_link_libraries(testthreadpool cppweb)

add_executable(testTimerLoop testTimerLoop.cpp)
target_link_libraries(testTimerLoop cppweb)

add_executable(testBuffer testBuffer.cpp)
target_link_libraries(testBuffer cppweb)

add_executable(DiscardServer DiscardServer.cpp)
target_link_libraries(DiscardServer cppweb)

add_executable(EchoClient EchoClient.cpp)
target_link_libraries(EchoClient cppweb)