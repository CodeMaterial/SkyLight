#include "speechNode.h"
#include "skylight_message/user_command.hpp"
#include "skylight_message/trigger.hpp"

skylight::SpeechNode::SpeechNode() {

    if (!mMessaging.good()) {
        throw std::runtime_error("messaging failed to initialise");
    }

    mConfig = skylight::GetConfig("skylight_speech.toml");

    auto [deviceStringSuccess, deviceString] = mConfig->getString("device");

    if (!deviceStringSuccess) {
        throw std::runtime_error("failed to grab device name from config file");
    }

    if (!mSpeech.Connect(deviceString)) {
        throw std::runtime_error("speech failed to connect to device");
    }

    mMessaging.subscribe("speech/start", &skylight::SpeechNode::OnStart, this);
    mMessaging.subscribe("speech/stop", &skylight::SpeechNode::OnStop, this);

    mMessaging.Start();

}

skylight::SpeechNode::~SpeechNode() {
    mMessaging.Stop();
}

void skylight::SpeechNode::OnStart(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                   const skylight_message::trigger *msg) {
    spdlog::info("starting speech recognition");
    mSpeech.Start();

}

void skylight::SpeechNode::OnStop(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                  const skylight_message::trigger *msg) {
    spdlog::info("stopping speech recognition");
    skylight_message::user_command userCommand;
    std::string speechDetected = mSpeech.Stop();
    if (!speechDetected.empty()) {
        userCommand.command = speechDetected;
        spdlog::info("speech detected: {}", speechDetected);
        mMessaging.publish("speech/command", &userCommand);
    }
    if (speechDetected.empty()) {
        spdlog::info("no speech detected");
    }
}