cmake_minimum_required(VERSION 3.6)
project(online_hga)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.c
        prepare_data.c prepare_data.h
        tools.c tools.h
        structure.h hga.c hga.h commen.c commen.h heuristic_func.c heuristic_func.h pga.c pga.h)

add_executable(ONLINEHGA ${SOURCE_FILES})