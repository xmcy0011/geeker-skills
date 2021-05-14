cmake_minimum_required(VERSION 3.17)
project(05_simple_multi_thread_in_ssl)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(client)
add_subdirectory(server)

if(APPLE)
    include_directories(/usr/local/Cellar/openssl@1.1/1.1.1j/include)
    link_directories(/usr/local/Cellar/openssl@1.1/1.1.1j/lib)
endif()

link_libraries(crypto ssl)
add_executable(ssl_client client/ssl_client.cpp)

# openssl
link_libraries(crypto ssl)
add_executable(ssl_server server/ssl_server.cpp)