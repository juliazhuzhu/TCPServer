cmake_minimum_required(VERSION 3.19)
project(TCPServer)
set(CMAKE_CXX_STANDARD 14)

add_executable(TCPServer epoll_server.cpp)

add_executable(TCPClient tcp_client.cpp)

FIND_LIBRARY(event libevent.a /usr/local/libevent/lib)
include_directories("/usr/local/libevent/include")
add_executable(LibEVentServer libevent_server.cpp)
target_link_libraries(LibEVentServer ${event})