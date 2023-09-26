#pragma once

#include <atomic>

#include "speech.h"
#include "skylight_messaging.h"
#include "skylight_config.h"
#include "skylight_message/simple_void.hpp"
#include "skylight_message/simple_string.hpp"


namespace skylight {

    class SpeechNode {
    public:
        SpeechNode();

        ~SpeechNode();

        void
        SetJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_string *msg);

        void
        ResetJSGF(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_string *msg);

    private:

        void
        OnStart(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_void *msg);

        void OnStop(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_void *msg);

        skylight::Messaging mMessaging;
        std::shared_ptr<Speech> mpSpeech;

        std::atomic_bool mJsgfUpdated = false;
        std::filesystem::path mJsgfFile;
    };
}