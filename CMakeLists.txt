cmake_minimum_required(VERSION 3.18)
project(PathTracer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PathTracer
    src/main.cpp
)

target_include_directories(PathTracer PRIVATE include)