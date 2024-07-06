cmake_minimum_required(VERSION 3.28)
project(rockyou_reader C)

set(CMAKE_C_STANDARD 11)

add_library(rockyou_reader STATIC library.c
        library.h)
