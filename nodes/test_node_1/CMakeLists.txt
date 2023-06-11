cmake_minimum_required(VERSION 3.18)
project(test_node_1)

set(CMAKE_CXX_STANDARD 20)

find_package(lcm REQUIRED)
include(${LCM_USE_FILE})

lcm_wrap_types(
        CPP_HEADERS test_node_1_headers
        ../message_types/button_t.lcm
)

lcm_add_library(test_node_1_messages CPP ${test_node_1_headers})
target_include_directories(test_node_1_messages INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


add_executable(test_node_1 main.cpp ../common/messaging.h)
lcm_target_link_libraries(test_node_1 test_node_1_messages ${LCM_NAMESPACE}lcm tomlcpp)
