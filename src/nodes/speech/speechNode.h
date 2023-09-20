#pragma once

#include "speech.h"
#include "skylight_messaging.h"
#include "skylight_config.h"
#include "skylight_message/simple_void.hpp"


namespace skylight {

    class SpeechNode {
    public:
        SpeechNode();

        ~SpeechNode();

    private:

        void
        OnStart(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_void *msg);

        void OnStop(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_void *msg);

        skylight::Messaging mMessaging;
        skylight::Config mConfig;
        Speech mSpeech;
    };
}