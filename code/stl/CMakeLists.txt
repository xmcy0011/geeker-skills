cmake_minimum_required(VERSION 3.15)
project(stl)

set(CMAKE_CXX_STANDARD 14)
set(src_list ./)

aux_source_directory(app src_list)

#include_directories(app)
#link_directories(app)

add_executable(stl main.cpp ${src_list})