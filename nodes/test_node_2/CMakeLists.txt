cmake_minimum_required(VERSION 3.18)
project(test_node_2)

set(CMAKE_CXX_STANDARD 20)


find_package(lcm REQUIRED)
include(${LCM_USE_FILE})

add_executable(test_node_2 main.cpp)

lcm_target_link_libraries(test_node_2 ${LCM_NAMESPACE}lcm)
