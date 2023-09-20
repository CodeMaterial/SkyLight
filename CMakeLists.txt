cmake_minimum_required(VERSION 3.18)

project(SkyLight)

add_subdirectory(src/nodes/cli)
add_subdirectory(src/nodes/gpio)
add_subdirectory(src/nodes/effect_driver)
add_subdirectory(src/nodes/effect_driver_and_gpio)
add_subdirectory(src/nodes/speech)

install(FILES src/config/skylight_topic_config.toml
        DESTINATION /usr/local/etc)