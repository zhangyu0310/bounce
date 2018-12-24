cmake_minimum_required(VERSION 3.13)
project(bounce_tests)

set(CMAKE_CXX_STANDARD 11)

add_executable(buffer_test
        buffer_test.cc)
target_link_libraries(buffer_test bounce)
target_link_libraries(buffer_test gtest_main)
target_link_libraries(buffer_test gtest)
target_link_libraries(buffer_test gtest_main)
target_link_libraries(buffer_test pthread)

add_executable(poller_test poller_test.cc)
target_link_libraries(poller_test bounce)
target_link_libraries(poller_test pthread)

add_executable(tcpconnection_test tcpconnection_test.cc)
target_link_libraries(tcpconnection_test bounce)
target_link_libraries(tcpconnection_test pthread)

add_executable(tcpserver_test tcpserver_test.cc)
target_link_libraries(tcpserver_test bounce)
target_link_libraries(tcpserver_test pthread)

add_executable(shutdown_test shutdown_test.cc)
target_link_libraries(shutdown_test bounce)
target_link_libraries(shutdown_test pthread)

add_executable(echo_bench echo_bench.cc)
target_link_libraries(echo_bench bounce)
target_link_libraries(echo_bench pthread)

add_executable(timer_test timer_test.cc)
target_link_libraries(timer_test bounce)
target_link_libraries(timer_test pthread)

add_executable(connector_test connector_test.cc)
target_link_libraries(connector_test bounce)
target_link_libraries(connector_test pthread)