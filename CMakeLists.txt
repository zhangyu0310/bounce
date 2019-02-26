cmake_minimum_required(VERSION 3.13)
project(bounce)

include(./bounce_config.cmake)

add_subdirectory(bounce)

# Install
set(DEP_HEADERS)

install(DIRECTORY deps/spdlog DESTINATION include)
install(DIRECTORY deps/any DESTINATION include)