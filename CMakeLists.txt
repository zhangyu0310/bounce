cmake_minimum_required(VERSION 3.13)
project(bounce)

include(./bounce_config.cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-O0 -Wall -g -ggdb3")
else()
    set(CMAKE_CXX_FLAGS "-O2 -Wall -DNDEBUG")
endif()

message("-- CMake build type is " ${CMAKE_BUILD_TYPE})
add_subdirectory(bounce)

# Install
set(DEP_HEADERS)

install(DIRECTORY deps/spdlog DESTINATION include)
install(DIRECTORY deps/any DESTINATION include)
