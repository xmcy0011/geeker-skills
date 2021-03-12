cmake_minimum_required(VERSION 3.17)
project(client)

set(CMAKE_CXX_STANDARD 14)

if (UNIX)
    link_libraries(pthread)
endif ()

add_executable(client main.cpp)