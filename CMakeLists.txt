cmake_minimum_required(VERSION 3.19)
project(TCPServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(TCPServer epoll_server.cpp)
add_executable(TCPClient tcp_client.cpp)