cmake_minimum_required(VERSION 3.12)

project(STL)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TEST_APP CACHE OFF)

add_library(stl STATIC "src/allocator.cpp")
target_include_directories(stl PUBLIC "include")

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    target_compile_options(stl PRIVATE "-Wall" "-Werror")
endif()

if (BUILD_TEST_APP)
    add_executable(App "src/main.cpp")
    target_link_libraries(App PUBLIC stl)
    target_include_directories(App PUBLIC "include")
endif()