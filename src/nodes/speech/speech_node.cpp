#include "speech_node.h"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_void.hpp"

skylight::SpeechNode::SpeechNode() {

    spdlog::info("skylight speech node initialising");

    if (!mMessaging.good()) {
        throw std::runtime_error("skylight speech node failed to initialise messaging");
    }

    skylight::Config pConfig = skylight::GetConfig("skylight_speech.toml");

    std::string device = skylight::getConfigString(pConfig, "device");

    mpSpeech = std::make_shared<Speech>(fmt::format("sysdefault:CARD={}", device));

    mMessaging.subscribe("/speech/start", &skylight::SpeechNode::OnStart, this);
    mMessaging.subscribe("/speech/stop", &skylight::SpeechNode::OnStop, this);
    mMessaging.subscribe("/speech/jsgf", &skylight::SpeechNode::SetJSGF, this);

    mMessaging.Start();

    spdlog::info("skylight speech node initialised");

}

skylight::SpeechNode::~SpeechNode() {
    mMessaging.Stop();
}

void
skylight::SpeechNode::SetJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                              const skylight_message::simple_string *msg) {
    spdlog::info("skylight speech node setting new jsgf file {}", msg->data);
    mpSpeech->SetJSGF(msg->data);
}


void skylight::SpeechNode::OnStart(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                   const skylight_message::simple_void *msg) {
    spdlog::info("skylight speech node starting recording");
    mpSpeech->Start();

}

void skylight::SpeechNode::OnStop(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                  const skylight_message::simple_void *msg) {
    spdlog::info("skylight speech node stopping recording");
    skylight_message::simple_string userCommand;
    std::string speechDetected = mpSpeech->Stop();
    if (!speechDetected.empty()) {
        userCommand.data = speechDetected;
        spdlog::info("skylight speech detected speech: {}", speechDetected);
        mMessaging.publish("/speech/command", &userCommand);
    }
    if (speechDetected.empty()) {
        spdlog::info("skylight speech did not detect any speech");
    }
}