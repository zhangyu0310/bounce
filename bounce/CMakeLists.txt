cmake_minimum_required(VERSION 3.13)
project(bounce)

include(../bounce_config.cmake)

aux_source_directory(. BOUNCE_SRC_FILES)
add_library(bounce
        ${BOUNCE_SRC_FILES})

target_link_libraries(bounce pthread)

add_subdirectory(test)