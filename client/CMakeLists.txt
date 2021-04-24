cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)

add_executable(client main.cpp client.cpp client.hpp master.hpp reactor.cpp reactor.hpp)
target_link_libraries(client PRIVATE Threads::Threads )
