cmake_minimum_required(VERSION 4.2)
project(ray_marching_demo
    VERSION 0.0.1
    LANGUAGES CXX
)

add_executable(ray_marching_demo main.cpp)

target_precompile_headers(ray_marching_demo PRIVATE pch.h)