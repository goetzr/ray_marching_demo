cmake_minimum_required(VERSION 4.2)
project(ray_marching_demo
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Require C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

add_executable(ray_marching_demo main.cpp)

target_precompile_headers(ray_marching_demo PRIVATE pch.h)

# Declare the external SDL3 library content
FetchContent_Declare(
    sdl3
    URL      https://github.com/libsdl-org/SDL/releases/download/release-3.4.0/SDL3-3.4.0.tar.gz
    URL_HASH SHA256=082cbf5f429e0d80820f68dc2b507a94d4cc1b4e70817b119bbb8ec6a69584b8
)

# Make the SDL3 library available and process its CMakeLists.txt file
FetchContent_MakeAvailable(sdl3)

target_link_libraries(ray_marching_demo PRIVATE SDL3::SDL3)