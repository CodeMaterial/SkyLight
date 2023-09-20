#include "speech_node.h"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_void.hpp"

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
                                   const skylight_message::simple_void *msg) {
    spdlog::info("starting speech recognition");
    mSpeech.Start();

}

void skylight::SpeechNode::OnStop(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                  const skylight_message::simple_void *msg) {
    spdlog::info("stopping speech recognition");
    skylight_message::simple_string userCommand;
    std::string speechDetected = mSpeech.Stop();
    if (!speechDetected.empty()) {
        userCommand.data = speechDetected;
        spdlog::info("speech detected: {}", speechDetected);
        mMessaging.publish("speech/command", &userCommand);
    }
    if (speechDetected.empty()) {
        spdlog::info("no speech detected");
    }
}