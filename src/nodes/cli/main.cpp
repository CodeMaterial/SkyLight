#include <iostream>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"
#include "skylight_time.h"

#include "skylight_message/user_command.hpp"
#include "skylight_message/trigger.hpp"


int main(int argc, char **argv) {

    CLI::App app{"App description"};

    std::string broadcastChannel = "command";
    app.add_option("-c,--channel", broadcastChannel, "The channel to broadcast the command")->required();

    CLI11_PARSE(app, argc, argv);

    skylight::Messaging messaging;

    if (!messaging.good())
    {
        spdlog::error("cli system failed to start the messaging system");
        return 1;
    }


    do {
        std::cout << "Command to send to '" << broadcastChannel << "': ";
        std::string userInput;
        std::getline(std::cin, userInput);
        if(userInput.empty())
        {
            skylight_message::trigger trigger;
            messaging.publish(broadcastChannel, &trigger);
        }
        if(!userInput.empty())
        {
            skylight_message::user_command userCommand;
            userCommand.command = userInput;
            userCommand.timestamp = skylight::Now();
            messaging.publish(broadcastChannel, &userCommand);
        }
    } while (std::cin.get() == '\n');

    return 0;
}


