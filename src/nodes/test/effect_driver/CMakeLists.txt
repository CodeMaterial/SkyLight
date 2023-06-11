cmake_minimum_required(VERSION 3.18)
project(effect_driver)

set(CMAKE_CXX_STANDARD 20)

find_package(lcm REQUIRED)
include(${LCM_USE_FILE})

lcm_wrap_types(
        CPP_HEADERS effect_driver_headers
        ../../../message_types/test/user_command.lcm
)

lcm_add_library(effect_driver_messages CPP ${effect_driver_headers})
target_include_directories(effect_driver_messages INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ../../../common)


add_executable(effect_driver effect_driver.cpp)
lcm_target_link_libraries(effect_driver effect_driver_messages ${LCM_NAMESPACE}lcm tomlcpp)
