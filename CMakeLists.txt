cmake_minimum_required(VERSION 3.5)

project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O0 -fsanitize=address")
set(CMAKE_EXPORT_COMPILE_COMMANDS:BOOL ON)

set(resources src/resources)

file(GLOB_RECURSE server_sources src/server/*.cpp
    ${resources}/thread_pool.cpp ${resources}/event.cpp
    ${resources}/tcp_socket.cpp ${resources}/unix_socket.cpp)

add_executable(server ${server_sources})

target_include_directories(server PUBLIC include/server include/resources)

file(GLOB_RECURSE resources_sources src/resources/*.cpp)

add_executable(resources ${resources_sources})
target_include_directories(resources PUBLIC include)

