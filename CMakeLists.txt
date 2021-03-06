cmake_minimum_required(VERSION 3.19)
project(TCPServer)
set(CMAKE_CXX_STANDARD 14)

add_executable(TCPServer epoll_server.cpp)

add_executable(TCPClient tcp_client.cpp)

add_executable(UDPClient udp_client.cpp)

if (CMAKE_HOST_UNIX)
    FIND_LIBRARY(event libevent.a /usr/local/libevent/lib)
    include_directories("/usr/local/libevent/include")
    add_executable(LibEVentServer libevent_server.cpp)
    target_link_libraries(LibEVentServer ${event})

    add_executable(LibEVentUdpServer libevent_updserver.cpp)
    target_link_libraries(LibEVentUdpServer ${event})

    FIND_LIBRARY(libuv libuv.a /usr/local/libuv/lib)
    include_directories("/usr/local/libuv/include")
    add_executable(LibUVUdpServer libuv_updserver.cpp)
    target_link_libraries(LibUVUdpServer ${libuv} pthread)
endif()

add_executable(TestSig testsig.cpp)
add_executable(TestSigAction testsigaction.cpp)
add_executable(TestDaemon testdaemon.cpp)
add_executable(DaemonApp daemon_app.cpp)