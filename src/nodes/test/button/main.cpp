#include <iostream>
#include "spdlog/spdlog.h"
#include "skylight_message/user_command.hpp"
#include "skylight_messaging.h"
#include "skylight_time.h"



int main(int argc, char **argv) {
    skylight::Messaging messaging;
    if (!messaging.good())
        return 1;

    do {
        skylight_message::user_command button_press;
        button_press.timestamp = skylight::Now();
        spdlog::info("Press a key to simulate a button push broadcasting to the button_press event");
        messaging.publish("button_press", &button_press);
    } while (std::cin.get() == '\n');

    return 0;
}