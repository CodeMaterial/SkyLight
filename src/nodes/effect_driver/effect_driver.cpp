#include "effect_driver.h"
#include "spdlog/spdlog.h"

skylight::EffectDriver::EffectDriver() {
    spdlog::info("Effect driver connecting...");

    // lets set everything up and grab the basic things from the config

    if (!mMessaging.good()) {
        throw std::runtime_error("effect driver failed to start the messaging system");
    }

    mpConfig = skylight::GetConfig("skylight_effect_driver.toml");

    auto [ledsPerStripValid, ledsPerStrip] = mpConfig->getInt("leds_per_strip");
    auto [channelCountValid, channelCount] = mpConfig->getInt("channels");

    mLedsPerStrip = static_cast<int>(ledsPerStrip);
    mChannelCount = static_cast<int>(channelCount);

    spdlog::info("initialised effect driver with {} leds per channel and {} channels", mLedsPerStrip, mChannelCount);

    if (!ledsPerStripValid || !channelCountValid) {
        throw std::runtime_error("effect driver failed to read the leds_per_strip or channels");
    }

    auto [refreshRateValid, refreshRate] = mpConfig->getInt("refresh_rate");
    if (!refreshRateValid) {
        throw std::runtime_error("effect driver failed to read the refresh_rate ");
    }

    mDelay = std::chrono::microseconds(1'000'000 / refreshRate);

    // clear the buffers

    std::fill(std::begin(mBuffer.buffer), std::begin(mBuffer.buffer) + mLedsPerStrip * mChannelCount * 3, 0);

    std::fill(std::begin(mBuffer.enabledChannels), std::begin(mBuffer.enabledChannels) + mChannelCount, 1);

    // subscribe to everything we need to

    mMessaging.subscribe("effect_driver/effects/test/start", &EffectDriver::TestEffect, this);

    // and we're done!

    spdlog::info("Effect driver connected...");
}

skylight::EffectDriver::~EffectDriver() {
}

void skylight::EffectDriver::Start() {
    mMessaging.Start();
}

void skylight::EffectDriver::RegisterBufferPublishOverride(
        std::function<void(const skylight_message::pixel_buffer *)> bufferOverrideFunc,
        std::function<void()> updateOverrideFunc) {
    spdlog::info("registering hardware publishing overrides");
    mBufferOverrideFunc = bufferOverrideFunc;
    mUpdateOverrideFunc = updateOverrideFunc;
}


void skylight::EffectDriver::TestEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                        const skylight_message::simple_void *msg) {

    spdlog::info("Starting test effect");

    std::chrono::time_point effectStart = std::chrono::steady_clock::now();
    std::chrono::time_point effectEnd = effectStart + std::chrono::seconds(2);

    auto [refreshRateValid, refreshRate] = mpConfig->getInt("refresh_rate");
    if (!refreshRateValid) return;

    int i = 0;
    int j = 0;

    mNextFrameTimestamp = std::chrono::steady_clock::now();
    BlackNow();

    while (mNextFrameTimestamp < effectEnd) {
        // do stuff to the buffer here referencing mNextFrameTimestamp for the "send" time
        {
            mBuffer.buffer[i] = j;
            j += 10;
            if (j > 255) {
                j = 0;
                i++;
            }
        }

        PublishLedBuffer();
        WaitUntilNextFrame();
        UpdateLedBuffer();
    }
    BlackNow();

    spdlog::info("test effect stopped");

}

bool skylight::EffectDriver::PublishLedBuffer() {

    //if we've got a copy of the gpio lib, send the effect straight to the spi buffer
    if (mBufferOverrideFunc) {
        mBufferOverrideFunc(&mBuffer);
    }

    if (!mBufferOverrideFunc) {
        mMessaging.publish("effect_driver/led_buffer", &mBuffer);
    }

    spdlog::info("led buffer published");

    return true;
}

bool skylight::EffectDriver::UpdateLedBuffer() {
    if (mUpdateOverrideFunc) {
        mUpdateOverrideFunc();
    }

    if (!mUpdateOverrideFunc) {
        mMessaging.publish("effect_driver/update", &mBuffer);
    }

    spdlog::info("led buffer update published");

    return true;
}

void skylight::EffectDriver::BlackNow() {
    spdlog::info("setting leds to black");

    std::fill(std::begin(mBuffer.buffer), std::begin(mBuffer.buffer) + mLedsPerStrip * mChannelCount * 3, 0);

    PublishLedBuffer();
    WaitUntilNextFrame();
    UpdateLedBuffer();
}

void skylight::EffectDriver::WaitUntilNextFrame() {
    spdlog::info("waiting for next frame");
    std::this_thread::sleep_until(mNextFrameTimestamp - std::chrono::milliseconds(1)); // sleepy wait
    while (std::chrono::steady_clock::now() < mNextFrameTimestamp); // busy wait
    mNextFrameTimestamp = std::chrono::steady_clock::now() + mDelay;
}