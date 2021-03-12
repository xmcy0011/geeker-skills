cmake_minimum_required(VERSION 3.17)
project(server)

set(CMAKE_CXX_STANDARD 14)
add_definitions(-g)

link_libraries(pthread)

add_executable(server main.cpp)