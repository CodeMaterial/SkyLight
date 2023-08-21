cmake_minimum_required(VERSION 3.18)

project(SkyLight)

add_subdirectory(src/nodes/button)
add_subdirectory(src/nodes/cli)

install(FILES src/config/topic_config.toml
        DESTINATION /usr/local/etc)