cmake_minimum_required(VERSION 4.2)
project(raymarcher_cpu
    VERSION 0.0.1
    LANGUAGES CXX
)

# Require C++ 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

add_executable(raymarcher_cpu
    main.cpp
    util.cpp
    camera.cpp
    object.cpp
    scene.cpp
    cube.cpp
    box2d.cpp
)

target_precompile_headers(raymarcher_cpu PRIVATE pch.h)

# Declare the external SDL3 library content
FetchContent_Declare(
    sdl3
    URL      https://github.com/libsdl-org/SDL/releases/download/release-3.4.0/SDL3-3.4.0.tar.gz
    URL_HASH SHA256=082cbf5f429e0d80820f68dc2b507a94d4cc1b4e70817b119bbb8ec6a69584b8
)

# Make the SDL3 library available and process its CMakeLists.txt file
FetchContent_MakeAvailable(sdl3)

target_link_libraries(raymarcher_cpu PRIVATE SDL3::SDL3)