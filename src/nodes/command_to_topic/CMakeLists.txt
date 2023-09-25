cmake_minimum_required(VERSION 3.18)
project(skylight_command_to_topic)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(SKYLIGHT_ROOT ../../..)

find_package(lcm REQUIRED)
find_package(spdlog REQUIRED)

include(${LCM_USE_FILE})

lcm_wrap_types(
        CPP_HEADERS ${PROJECT_NAME}_headers
        ${SKYLIGHT_ROOT}/src/message_types/simple_string.lcm
        ${SKYLIGHT_ROOT}/src/message_types/simple_void.lcm
        ${SKYLIGHT_ROOT}/src/message_types/simple_float.lcm

)

lcm_add_library(${PROJECT_NAME}_messages CPP ${${PROJECT_NAME}_headers})

target_include_directories(${PROJECT_NAME}_messages INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${SKYLIGHT_ROOT}/src/common)


add_executable(${PROJECT_NAME} main.cpp)
lcm_target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_messages ${LCM_NAMESPACE}lcm tomlcpp spdlog::spdlog)

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})