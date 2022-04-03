cmake_minimum_required(VERSION 3.21)
project(ransomware)

set(CMAKE_CXX_STANDARD 14)

add_executable(ransomware
        ransomware.cpp)

target_link_libraries(ransomware cryptopp stdc++fs)
