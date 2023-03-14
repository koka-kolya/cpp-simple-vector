cmake_minimum_required(VERSION 3.24)
project(SimpleVector)

set(CMAKE_CXX_STANDARD 17)

add_executable(SearchServer
    main.cpp
    array_ptr.h
    simple_vector.h
)
