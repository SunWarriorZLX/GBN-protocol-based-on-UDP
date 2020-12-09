cmake_minimum_required(VERSION 3.17)
project(UDP_GBN_LIB C)

set(CMAKE_C_STANDARD 11)

add_library(UDP_GBN_LIB SHARED udp_gbn.c udp_gbn.h)