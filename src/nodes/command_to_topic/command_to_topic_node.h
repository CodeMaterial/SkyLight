#pragma once

#include "command_storage.h"
#include "skylight_messaging.h"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_void.hpp"
#include <filesystem>

namespace skylight {

    class CommandToTopicNode {
    public:
        CommandToTopicNode();

        ~CommandToTopicNode();

        void RegisterOption(std::string optionID, std::vector<std::string> options);

        void RegisterCommand(std::string command, command_callback func);

        void Ready();

    private:


        void SendJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                      const skylight_message::simple_void *msg);

        void HandleCommand(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                           const skylight_message::simple_string *msg);

        skylight::CommandStore mCommandStorage;
        skylight::Messaging mMessaging;
        bool mReady = false;
    };
}