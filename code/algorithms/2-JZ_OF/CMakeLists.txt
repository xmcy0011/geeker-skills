cmake_minimum_required(VERSION 3.15)
project(2_JZ_OFFER)

set(CMAKE_CXX_STANDARD 11)
set(src_list ./)

#aux_source_directory(1-operator_function src_list)
#aux_source_directory(2-singleton src_list)
#aux_source_directory(3-duplicate_array src_list)
#aux_source_directory(4-matrix_array_find src_list)
#aux_source_directory(5-string_replace_space src_list)
#aux_source_directory(6-linklist_print_from_end)

# 要调试那个，放那个的main.cpp
#add_executable(JZ_OFFER 6-linklist_print_from_end/main.cpp ${src_list})
add_executable(JZ_OFFER 7-btree_recreate/main.cpp ${src_list})