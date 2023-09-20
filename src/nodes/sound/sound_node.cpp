#include "sound_node.h"
#include <exception>

skylight::SoundNode::SoundNode(std::string name) {

    mName = name;

    skylight::Config config = skylight::GetConfig("skylight_sound.toml")->getTable(mName);

    if (!config) {
        throw std::runtime_error("failed to get the table from the sound config");
    }

    bool enabled = skylight::getConfigInt(config, "enabled");

    if (enabled) {
        std::string card = skylight::getConfigString(config, "card");
        std::string volumeControl = skylight::getConfigString(config, "volume_control");
        double defaultVolume = skylight::getConfigDouble(config, "default_volume");
        mSound.Connect(card, volumeControl, static_cast<float>(defaultVolume));

        mMessaging.subscribe("/sound/" + mName + "/speak", &skylight::SoundNode::PlaySpeech, this);
        mMessaging.subscribe("/sound/" + mName + "/play", &skylight::SoundNode::PlayAudio, this);
        mMessaging.subscribe("/sound/" + mName + "/volume", &skylight::SoundNode::SetVolume, this);

        mMessaging.Start();
    }

}

skylight::SoundNode::~SoundNode() {

}

void skylight::SoundNode::PlaySpeech(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                     const skylight_message::simple_string *msg) {

    spdlog::info("Sound Node {} speaking: {}", mName, msg->data);
    mSound.Say(msg->data);
}

void skylight::SoundNode::PlayAudio(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                    const skylight_message::simple_string *msg) {

    spdlog::info("Sound Node {} playing audio: {}", mName, msg->data);
    mSound.PlayAudio(msg->data);
}

void skylight::SoundNode::SetVolume(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                    const skylight_message::simple_float *msg) {

    spdlog::info("Sound Node {} setting volume to: {}", mName, msg->data);
    mSound.SetVolume(msg->data);
}