#include <iostream>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"

#include "skylight_message/simple_void.hpp"
#include "skylight_message/simple_string.hpp"
#include <stdexcept>


int main(int argc, char **argv) {

    CLI::App app{"App description"};

    std::string broadcastChannel;
    bool trigger;
    std::string commandString = "";

    app.add_option("-c,--channel", broadcastChannel, "The channel to broadcast the command")->required();

    app.add_option("-s, --string", commandString, "send a string command");

    CLI11_PARSE(app, argc, argv);

    skylight::Messaging messaging;

    if (!messaging.good()) {
        throw std::runtime_error("cli system failed to start the messaging system");
    }

    if (commandString.empty()) {
        skylight_message::simple_void trigger;
        spdlog::info("Sending trigger to {}", broadcastChannel);
        messaging.publish(broadcastChannel, &trigger);
    }
    if (!commandString.empty()) {
        skylight_message::simple_string stringCommand;
        stringCommand.data = commandString;
        spdlog::info("Sending user command {} to {}", commandString, broadcastChannel);
        messaging.publish(broadcastChannel, &stringCommand);
    }

    return 0;
}


