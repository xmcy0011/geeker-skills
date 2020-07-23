cmake_minimum_required(VERSION 3.15)
project(2_JZ_OFFER)

set(CMAKE_CXX_STANDARD 11)
set(src_list ./)

#aux_source_directory(1-operator_function src_list)
#aux_source_directory(2-singleton src_list)
#aux_source_directory(3-duplicate_array src_list)
#aux_source_directory(4-matrix_array_find src_list)

add_executable(JZ_OFFER 4-matrix_array_find/main.cpp ${src_list})