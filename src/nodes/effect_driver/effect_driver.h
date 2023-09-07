#pragma once

#include <memory>
#include <functional>
#include <chrono>
#include "skylight_messaging.h"
#include "skylight_message/trigger.hpp"
#include "skylight_message/pixel_buffer.hpp"

namespace skylight {

    class EffectDriver {
    public:
        EffectDriver();

        ~EffectDriver();

        void Start();

        void
        RegisterBufferPublishOverride(std::function<void(const skylight_message::pixel_buffer *)> bufferOverrideFunc,
                                      std::function<void()> updateOverrideFunc);

        void TestEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::trigger *msg);

        bool PublishLedBuffer();

        bool UpdateLedBuffer();

        void BlackNow();

    private:

        void WaitUntilNextFrame();

        skylight::Messaging mMessaging;
        std::shared_ptr<toml::Table> mpConfig;

        skylight_message::pixel_buffer mBuffer;

        std::function<void(const skylight_message::pixel_buffer *)> mBufferOverrideFunc;
        std::function<void()> mUpdateOverrideFunc;

        std::chrono::time_point<std::chrono::steady_clock> mNextFrameTimestamp;

        std::chrono::microseconds mDelay;

        int mLedsPerStrip;
        int mChannelCount;
    };

}

