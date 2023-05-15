cmake_minimum_required(VERSION 3.7)
project("cppweb")

add_definitions("-std=c++14 -g")#定义编译器参数

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(cppweb)
add_subdirectory(test)
