cmake_minimum_required(VERSION 3.10)
project(webcam_test C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)

find_package(SDL3 REQUIRED)

# set release as default build
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE release CACHE STRING "build type (debug/release)" FORCE)
endif()

file(GLOB SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_include_directories(${PROJECT_NAME} PRIVATE include ${SDL3_INCLUDE_DIRS})

# debug mode stuffs
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -Wall -Wextra)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()
