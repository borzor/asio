cmake_minimum_required(VERSION 3.5)

project(tcp_echo_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_echo_server main.cpp server.cpp)
