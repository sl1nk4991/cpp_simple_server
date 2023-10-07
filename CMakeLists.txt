cmake_minimum_required(VERSION 3.5)

project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -O0 -fsanitize=address")
set(CMAKE_EXPORT_COMPILE_COMMANDS:BOOL ON)

file(GLOB_RECURSE server_sources src/server/*.cpp src/resources/thread_pool.cpp
    include/*.h include/resources/thread_pool.h)

add_executable(server ${server_sources})

target_include_directories(server PUBLIC include)

file(GLOB_RECURSE resources_sources src/resources/*.cpp include/resources/*.h)

add_executable(resources ${resources_sources})
target_include_directories(resources PUBLIC include)

