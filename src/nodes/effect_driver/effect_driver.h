#pragma once

#include <memory>
#include <functional>
#include <chrono>

#include "skylight_messaging.h"
#include "skylight_message/simple_void.hpp"
#include "skylight_message/pixel_buffer.hpp"

namespace skylight {

    class EffectDriver {
    public:
        EffectDriver(std::function<void(const skylight_message::pixel_buffer *)> onBuffer,
                     std::function<void()> onUpdate);

        ~EffectDriver();

        void
        TestEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_void *msg);

        void PublishLedBuffer();

        void UpdateLedBuffer();

        void BlackNow();

    private:

        void WaitUntilNextFrame();

        skylight_message::pixel_buffer mBuffer;

        std::function<void(const skylight_message::pixel_buffer *)> mOnBuffer;
        std::function<void()> mOnUpdate;

        std::chrono::time_point<std::chrono::steady_clock> mNextFrameTimestamp;

        std::chrono::microseconds mDelay;

        int mLedsPerStrip;
        int mChannelCount;
    };

}

