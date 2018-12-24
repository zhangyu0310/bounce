cmake_minimum_required(VERSION 3.13)
project(bounce)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
include_directories(.)
include_directories(deps)

add_subdirectory(bounce/test)

aux_source_directory(bounce BOUNCE_SRC)
add_library(bounce
        ${BOUNCE_SRC})

target_link_libraries(bounce pthread)