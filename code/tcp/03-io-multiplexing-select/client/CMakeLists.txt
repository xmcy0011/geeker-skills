cmake_minimum_required(VERSION 3.16)
project(client)

set(CMAKE_CXX_STANDARD 14)

if(UNIX)
    link_libraries(pthread)
else()

endif()

add_executable(client main.cpp)