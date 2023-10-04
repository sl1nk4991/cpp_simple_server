project(server)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -ggdb3")
add_executable(server main.cpp server.cpp handler.cpp thread_pool.cpp)
