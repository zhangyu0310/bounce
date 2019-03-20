# Compile options
set(CMAKE_CXX_STANDARD 11)

# Project Files path
set(PROJECT_PATH "${CMAKE_SOURCE_DIR}")
set(BOUNCE_SRC "${PROJECT_PATH}/bounce")
set(BOUNCE_TEST "${BOUNCE_SRC}/test")
set(BOUNCE_DEP "${PROJECT_PATH}/deps")

# Head files
include_directories(
        ${PROJECT_PATH}
        ${BOUNCE_SRC}
        ${BOUNCE_TEST}
        ${BOUNCE_DEP} )
