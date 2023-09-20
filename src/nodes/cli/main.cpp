#include <iostream>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"

#include "skylight_message/simple_void.hpp"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_float.hpp"

#include <stdexcept>


int main(int argc, char **argv) {

    CLI::App app{"App description"};

    std::string broadcastChannel;
    app.add_option("-c,--channel", broadcastChannel,
                   "The channel to broadcast on, sends a simple_void message if no other arguments are used")->required();

    std::string simpleStringValue;
    CLI::Option *simpleStringOption = app.add_option("-s, --string", simpleStringValue, "send a simple_string message");

    float simpleFloatValue;
    CLI::Option *simpleFloatOption = app.add_option("-f, --float", simpleFloatValue, "send a simple_float message");

    CLI11_PARSE(app, argc, argv);

    skylight::Messaging messaging;

    if (!messaging.good()) {
        throw std::runtime_error("cli system failed to start the messaging system");
    }

    if (simpleStringOption) {
        spdlog::info("skylight cli sending simple_string message \"{}\" to {}", simpleStringValue, broadcastChannel);
        skylight_message::simple_string simpleStringMessage;
        simpleStringMessage.data = simpleStringValue;
        messaging.publish(broadcastChannel, &simpleStringMessage);

    } else if (simpleFloatOption) {
        spdlog::info("skylight cli sending simple_float message {} to {}", simpleFloatValue, broadcastChannel);
        skylight_message::simple_float simpleFloatMessage;
        simpleFloatMessage.data = simpleFloatValue;
        messaging.publish(broadcastChannel, &simpleFloatMessage);

    } else {
        spdlog::info("skylight cli sending simple_void message to {}", broadcastChannel);
        skylight_message::simple_void simpleVoidMessage;
        messaging.publish(broadcastChannel, &simpleVoidMessage);
    }

    return 0;
}


