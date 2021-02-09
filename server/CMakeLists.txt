cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Boost 1.71 REQUIRED)
find_package(Threads REQUIRED)

add_executable(server main.cpp server.cpp)
target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server Threads::Threads)
