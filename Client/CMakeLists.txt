cmake_minimum_required(VERSION 3.17)
project(Client C)

set(CMAKE_C_STANDARD 11)
include_directories(../UDP_GBN_LIB)
link_directories(../UDP_GBN_LIB/cmake-build-debug)
link_libraries(UDP_GBN_LIB)

add_executable(Client main.c)

target_link_libraries(Client UDP_GBN_LIB)