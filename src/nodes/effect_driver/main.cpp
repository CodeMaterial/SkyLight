#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <pigpio.h>
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"
#include "skylight_config.h"

#include "skylight_message/pixel_buffer.hpp"
#include "skylight_message/trigger.hpp"


int main(int argc, char **argv) {

    skylight::Messaging messaging;
    if (!messaging.good())
    {
        spdlog::error("led driver system failed to instantiate messaging correctly");
        return 1;
    }

    std::shared_ptr<toml::Table> pConfig = skylight::GetConfig("skylight_effect_driver.toml");

    if(!pConfig)
    {
        spdlog::error("led driver system failed to load config");
        return 1;
    }

    return 0;
}