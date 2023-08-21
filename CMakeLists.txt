cmake_minimum_required(VERSION 3.18)

project(SkyLight)

add_subdirectory(src/nodes/button)
add_subdirectory(src/nodes/cli)
add_subdirectory(src/nodes/gpio)
add_subdirectory(src/nodes/effect_driver)

install(FILES src/config/topic_config.toml
        DESTINATION /usr/local/etc)