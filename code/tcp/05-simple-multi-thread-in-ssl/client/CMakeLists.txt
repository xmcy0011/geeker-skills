if(APPLE)
    include_directories(/usr/local/Cellar/openssl@1.1/1.1.1j/include)
    link_directories(/usr/local/Cellar/openssl@1.1/1.1.1j/lib)
endif()

# openssl
link_libraries(crypto ssl)
add_executable(client ssl_client.cpp)