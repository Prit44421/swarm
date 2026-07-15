cmake_minimum_required(VERSION 3.16)
project(swarm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
add_executable(swarm 
    src/main.cpp
    src/bencode.cpp
    src/torrent_file.cpp
    src/tracker.cpp
)

target_link_libraries(swarm PRIVATE CURL::libcurl OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(swarm PRIVATE src/)