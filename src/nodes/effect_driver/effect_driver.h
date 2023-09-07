#pragma once

#include <memory>
#include <functional>
#include "skylight_messaging.h"
#include "skylight_message/trigger.hpp"
#include "skylight_message/pixel_buffer.hpp"

namespace skylight {

    class EffectDriver {
    public:
        EffectDriver();

        ~EffectDriver();

        bool Connect();

        bool Start();

        void
        RegisterBufferPublishOverride(std::function<void(const skylight_message::pixel_buffer *)> bufferOverrideFunc);

        void TestEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::trigger *msg);

        bool PublishLedBuffer();


    private:
        skylight::Messaging mMessaging;
        std::shared_ptr<toml::Table> mpConfig;

        skylight_message::pixel_buffer mBuffer;

        std::function<void(const skylight_message::pixel_buffer *)> mBufferOverrideFunc;
    };

}

