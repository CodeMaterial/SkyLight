#pragma once

#include "sound.h"
#include "skylight_messaging.h"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_float.hpp"


namespace skylight {

    class SoundNode {
    public:
        SoundNode(std::string name);

        ~SoundNode();

        void PlaySpeech(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                        const skylight_message::simple_string *msg);

        void PlayAudio(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                       const skylight_message::simple_string *msg);

        void SetVolume(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                       const skylight_message::simple_float *msg);

    private:

        std::shared_ptr<Sound> mpSound;
        std::string mName;
        skylight::Messaging mMessaging;
    };
}