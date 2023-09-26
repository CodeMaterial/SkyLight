#include "speech_node.h"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_void.hpp"

skylight::SpeechNode::SpeechNode() {

    spdlog::info("skylight speech node initialising");

    if (!mMessaging.good()) {
        throw std::runtime_error("skylight speech node failed to initialise messaging");
    }

    skylight::Config pConfig = skylight::GetConfig("skylight_speech.toml");

    bool waitForJsgf = skylight::getConfigBool(pConfig, "wait_for_jsgf");

    std::string device = skylight::getConfigString(pConfig, "device");
    mMessaging.Start();

    if (waitForJsgf) {

        mMessaging.subscribe("/speech/jsgf", &skylight::SpeechNode::SetJSGF, this);

        while (mJsgfUpdated == false) {
            spdlog::info("waiting for /speech/jsgf");
            skylight_message::simple_void vMsg;
            mMessaging.publish("/speech/jsgf/query", &vMsg);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        mpSpeech = std::make_shared<Speech>(fmt::format("sysdefault:CARD={}", device), mJsgfFile);

    } else {

        mMessaging.subscribe("/speech/jsgf", &skylight::SpeechNode::ResetJSGF, this);
        mpSpeech = std::make_shared<Speech>(fmt::format("sysdefault:CARD={}", device));

    }

    mMessaging.subscribe("/speech/start", &skylight::SpeechNode::OnStart, this);
    mMessaging.subscribe("/speech/stop", &skylight::SpeechNode::OnStop, this);

    spdlog::info("skylight speech node initialised");

}

skylight::SpeechNode::~SpeechNode() {
    mMessaging.Stop();
}

void
skylight::SpeechNode::SetJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                              const skylight_message::simple_string *msg) {
    spdlog::info("skylight speech node setting new jsgf file {}", msg->data);
    mJsgfFile = msg->data;
    mJsgfUpdated = true;
}

void
skylight::SpeechNode::ResetJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                const skylight_message::simple_string *msg) {
    spdlog::info("skylight speech node setting new jsgf file {}", msg->data);
    mpSpeech->SetJSGF(msg->data);
    mJsgfFile = msg->data;
    mJsgfUpdated = true;
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