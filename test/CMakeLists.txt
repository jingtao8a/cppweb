add_executable(testlogger testlogger.cpp)
target_link_libraries(testlogger cppweb)

add_executable(testInetAddress testInetAddress.cpp)
target_link_libraries(testInetAddress cppweb)

add_executable(testthreadpool testthreadpool.cpp)
target_link_libraries(testthreadpool cppweb)