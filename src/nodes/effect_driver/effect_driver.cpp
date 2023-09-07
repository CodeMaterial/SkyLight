#include "effect_driver.h"
#include "spdlog/spdlog.h"

skylight::EffectDriver::EffectDriver() {
    spdlog::info("this is the effect driver initialiser");
}

skylight::EffectDriver::~EffectDriver() {
    spdlog::info("this is the effect driver destructor");
}

bool skylight::EffectDriver::Connect() {

    if (!mMessaging.good()) {
        spdlog::error("effect driver failed to initialise messaging");
        return false;
    }

    mpConfig = skylight::GetConfig("skylight_effect_driver.toml");

    if (!mpConfig) {
        spdlog::error("effect driver failed to read config");
        return false;
    }

    mMessaging.subscribe("effect_driver/effects/test/start", &EffectDriver::TestEffect, this);

    auto [ledsPerStripValid, ledsPerStrip] = mpConfig->getInt("leds_per_strip");
    auto [channelCountValid, channelCount] = mpConfig->getInt("channels");

    if (!ledsPerStripValid || !channelCountValid) {
        spdlog::error("effect driver failed to read the leds_per_strip or channels");
        return false;
    }

    std::fill(std::begin(mBuffer.buffer), std::begin(mBuffer.buffer) + ledsPerStrip * channelCount * 3, 0);

    std::fill(std::begin(mBuffer.enabledChannels), std::begin(mBuffer.enabledChannels) + channelCount, 1);

    return true;
}

bool skylight::EffectDriver::Start() {
    mMessaging.Start();
    return true;
}

void skylight::EffectDriver::RegisterBufferPublishOverride(
        std::function<void(const skylight_message::pixel_buffer *)> bufferOverrideFunc) {
    mBufferOverrideFunc = bufferOverrideFunc;
}


void skylight::EffectDriver::TestEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                        const skylight_message::trigger *msg) {


}

bool skylight::EffectDriver::PublishLedBuffer() {
    //if we've got a copy of the gpio lib, send the effect straight to the spi buffer
    if (mBufferOverrideFunc) {
        mBufferOverrideFunc(&mBuffer);
    }

    mMessaging.publish("effect_driver/led_buffer", &mBuffer);
    return true;
}