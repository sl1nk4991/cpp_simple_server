cmake_minimum_required(VERSION 3.5)

project(server)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "-Wall -O0 -fsanitize=address")

file(GLOB_RECURSE server_sources src/server/*.cpp include/*.h)

add_executable(server ${server_sources})

target_include_directories(server PUBLIC include)

