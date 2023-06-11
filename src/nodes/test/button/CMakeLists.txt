cmake_minimum_required(VERSION 3.18)
project(button)

set(CMAKE_CXX_STANDARD 20)

find_package(lcm REQUIRED)
include(${LCM_USE_FILE})

lcm_wrap_types(
        CPP_HEADERS button_headers
        ../../../message_types/test/user_command.lcm
)

lcm_add_library(button_messages CPP ${button_headers})
target_include_directories(button_messages INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ../../../common)


add_executable(button button.cpp)
lcm_target_link_libraries(button button_messages ${LCM_NAMESPACE}lcm tomlcpp)
