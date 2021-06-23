cmake_minimum_required(VERSION 3.17)
project(02_dfa_sensitivewd_filter)

set(CMAKE_CXX_STANDARD 14)
aux_source_directory(./ src_list)

add_executable(02_dfa_sensitivewd_filter main.cpp word_node.hpp ${src_list})