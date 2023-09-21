cmake_minimum_required(VERSION 3.18)
project(skylight_effect_driver_and_gpio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(SKYLIGHT_ROOT ../../..)

find_package(lcm REQUIRED)
find_package(spdlog REQUIRED)

################################################################################
### Find the pigpio shared libraries.

# Find the path to the pigpio includes.
find_path(pigpio_INCLUDE_DIR
        NAMES pigpio.h pigpiod_if.h pigpiod_if2.h
        HINTS /usr/local/include)

# Find the pigpio libraries.
find_library(pigpio_LIBRARY
        NAMES libpigpio.so
        HINTS /usr/local/lib)
find_library(pigpiod_if_LIBRARY
        NAMES libpigpiod_if.so
        HINTS /usr/local/lib)
find_library(pigpiod_if2_LIBRARY
        NAMES libpigpiod_if2.so
        HINTS /usr/local/lib)

# Set the pigpio variables to plural form to make them accessible for
# the paramount cmake modules.
set(pigpio_INCLUDE_DIRS ${pigpio_INCLUDE_DIR})
set(pigpio_INCLUDES ${pigpio_INCLUDE_DIR})

# Handle REQUIRED, QUIET, and version arguments
# and set the <packagename>_FOUND variable.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(pigpio
        DEFAULT_MSG
        pigpio_INCLUDE_DIR pigpio_LIBRARY pigpiod_if_LIBRARY pigpiod_if2_LIBRARY)

################################################################################

include(${LCM_USE_FILE})

lcm_wrap_types(
        CPP_HEADERS ${PROJECT_NAME}_headers
        ${SKYLIGHT_ROOT}/src/message_types/pixel_buffer.lcm
        ${SKYLIGHT_ROOT}/src/message_types/simple_void.lcm

)

lcm_add_library(${PROJECT_NAME}_messages CPP ${${PROJECT_NAME}_headers})

target_include_directories(${PROJECT_NAME}_messages INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${SKYLIGHT_ROOT}/src/common)


add_executable(${PROJECT_NAME} main.cpp ../effect_driver/effect_driver.cpp ../gpio/gpio.cpp ../effect_driver/effect_driver_node.cpp ../gpio/gpio_node.cpp ../gpio/spi.cpp)
lcm_target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_messages ${LCM_NAMESPACE}lcm tomlcpp spdlog::spdlog pigpio)

configure_file(${SKYLIGHT_ROOT}/src/config/service_template.service ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.service)

install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.service
        DESTINATION /etc/systemd/system)

install(FILES ../effect_driver/skylight_effect_driver.toml
        DESTINATION /usr/local/etc)

install(FILES ../gpio/skylight_gpio.toml
        DESTINATION /usr/local/etc)