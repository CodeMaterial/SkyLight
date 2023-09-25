#include "sound_node.h"
#include <exception>

skylight::SoundNode::SoundNode(std::string name) {
    mName = name;

    spdlog::info("skylight sound node \"{}\" initialising", mName);

    if (!mMessaging.good()) {
        throw std::runtime_error("skylight sound node failed to initialise messaging");
    }

    skylight::Config config = skylight::GetConfig("skylight_sound.toml")->getTable(mName);

    if (!config) {
        throw std::runtime_error(
                fmt::format("skylight sound node failed to find {} device information in the config... somehow", mName
                ));
    }

    bool enabled = skylight::getConfigInt(config, "enabled");

    if (enabled) {
        std::string card = skylight::getConfigString(config, "card");
        std::string volumeControl = skylight::getConfigString(config, "volume_control");
        double defaultVolume = skylight::getConfigDouble(config, "default_volume");
        mpSound = std::make_shared<Sound>(card, volumeControl, static_cast<float>(defaultVolume));

        mMessaging.subscribe("/sound/" + mName + "/say", &skylight::SoundNode::PlaySpeech, this);
        mMessaging.subscribe("/sound/" + mName + "/play", &skylight::SoundNode::PlayAudio, this);
        mMessaging.subscribe("/sound/" + mName + "/volume", &skylight::SoundNode::SetVolume, this);

        mMessaging.Start();
    }
    spdlog::info("skylight sound node \"{}\" initialised", mName);

}

skylight::SoundNode::~SoundNode() {
}

void skylight::SoundNode::PlaySpeech(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                     const skylight_message::simple_string *msg) {

    spdlog::info("skylight sound node \"{}\" speaking: {}", mName, msg->data);
    mpSound->Say(msg->data);
}

void skylight::SoundNode::PlayAudio(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                    const skylight_message::simple_string *msg) {

    spdlog::info("skylight sound node \"{}\" playing audio: {}", mName, msg->data);
    mpSound->PlayAudio(msg->data);
}

void skylight::SoundNode::SetVolume(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                    const skylight_message::simple_float *msg) {

    spdlog::info("skylight sound node \"{}\" setting volume to: {}", mName, msg->data);
    mpSound->SetVolume(msg->data);
}