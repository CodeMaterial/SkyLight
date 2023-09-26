#include "command_to_topic_node.h"

skylight::CommandToTopicNode::CommandToTopicNode() {

    mMessaging.subscribe("/command_to_topic/command", &CommandToTopicNode::HandleCommand, this);

    mMessaging.subscribe("/speech/jsgf/query", &skylight::CommandToTopicNode::SendJSGF, this)->setQueueCapacity(1);

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

void skylight::CommandToTopicNode::Ready() {
    mReady = true;
    SendJSGF(nullptr, "", nullptr);
}

void skylight::CommandToTopicNode::SendJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                            const skylight_message::simple_void *msg) {
    if (mReady) {
        std::filesystem::path path = "/tmp/skylight_grammar.jsgf";
        mCommandStorage.GenerateJSGF(path);
        skylight_message::simple_string jsgfUpdateMsg{0, path.c_str()};
        mMessaging.publish("/speech/jsgf", &jsgfUpdateMsg);
    }

}