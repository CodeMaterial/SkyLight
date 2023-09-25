#include "command_to_topic_node.h"
#include "skylight_message/simple_string.hpp"

skylight::CommandToTopicNode::CommandToTopicNode() {

    mMessaging.subscribe("/command_to_topic/command", &CommandToTopicNode::HandleCommand, this);

    mMessaging.Start();

}

skylight::CommandToTopicNode::~CommandToTopicNode() = default;

void skylight::CommandToTopicNode::HandleCommand(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                                 const skylight_message::simple_string *msg) {

    spdlog::info("handling command {}", msg->data);
    auto [callback, args] = mCommandStorage.GetFunc(msg->data);
    if (callback) {
        callback(args, static_cast<void *>(&mMessaging));
    } else {
        spdlog::info("could not find callback");
    }

}

void skylight::CommandToTopicNode::RegisterOption(std::string optionID, std::vector<std::string> options) {
    mCommandStorage.RegisterOption(optionID, options);
}

void skylight::CommandToTopicNode::RegisterCommand(std::string command, skylight::command_callback func) {
    mCommandStorage.RegisterCommand(command, func);
}

void skylight::CommandToTopicNode::SetJSGF(std::filesystem::path filepath) {
    mCommandStorage.GenerateJSGF(filepath);
    skylight_message::simple_string jsgfUpdateMsg;
    jsgfUpdateMsg.data = filepath.c_str();
    mMessaging.publish("/speech/jsgf", &jsgfUpdateMsg);
}
