#include "effect_driver.h"
#include "spdlog/spdlog.h"
#include "hsv_to_rgb.h"

skylight::EffectDriver::EffectDriver(std::function<void(const skylight_message::pixel_buffer *)> onBuffer,
                                     std::function<void()> onUpdate) {

    spdlog::info("skylight effect driver starting");

    mOnBuffer = onBuffer;
    mOnUpdate = onUpdate;

    skylight::Config pConfig = skylight::GetConfig("skylight_effect_driver.toml");

    mLedsPerStrip = skylight::getConfigInt(pConfig, "leds_per_strip");
    mChannelCount = skylight::getConfigInt(pConfig, "channels");

    mDelay = std::chrono::microseconds(1'000'000 / skylight::getConfigInt(pConfig, "refresh_rate"));

    std::fill(std::begin(mBuffer.buffer), std::begin(mBuffer.buffer) + mLedsPerStrip * mChannelCount * 3, 0);

    std::fill(std::begin(mBuffer.enabledChannels), std::begin(mBuffer.enabledChannels) + mChannelCount, 1);

    spdlog::info("skylight effect driver finished initialisation with {} leds per channel and {} channels",
                 mLedsPerStrip, mChannelCount);
}

skylight::EffectDriver::~EffectDriver() {
}


void skylight::EffectDriver::TestEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                        const skylight_message::simple_void *msg) {

    spdlog::info("skylight effect driver starting test effect");

    std::chrono::time_point effectStart = std::chrono::steady_clock::now();
    std::chrono::time_point effectEnd = effectStart + std::chrono::seconds(5);
    std::chrono::duration effectDuration = effectEnd - effectStart;

    mNextFrameTimestamp = std::chrono::steady_clock::now();
    BlackNow();
    BlackNow();

    int loopCount = 0;
    while (mNextFrameTimestamp < effectEnd) {
        // do stuff to the buffer here referencing mNextFrameTimestamp for the "send" time
        {
            for (int ledIndex = 0; ledIndex < mLedsPerStrip
                                              / 2; ledIndex++) {

                HSV hsv{255, 255, 255};

                float effectProgress =
                        static_cast<float>((mNextFrameTimestamp - effectStart).count()) / effectDuration.count();

                float temp = static_cast<float>(ledIndex) / (mLedsPerStrip / 2) + effectProgress;

                hsv.hue = 255 * temp;

                RGB rgb;
                hsv2rgb_rainbow(hsv, rgb);

                mBuffer.buffer[ledIndex * 3 + 0] = rgb.r;
                mBuffer.buffer[ledIndex * 3 + 1] = rgb.g;
                mBuffer.buffer[ledIndex * 3 + 2] = rgb.b;

                mBuffer.buffer[(400 - ledIndex) * 3 + 0] = rgb.r;
                mBuffer.buffer[(400 - ledIndex) * 3 + 1] = rgb.g;
                mBuffer.buffer[(400 - ledIndex) * 3 + 2] = rgb.b;
            }
        }
        loopCount++;

        PublishLedBuffer();
        WaitUntilNextFrame();
        UpdateLedBuffer();
    }
    BlackNow();
    BlackNow();

    spdlog::info("skylight effect driver test effect ended");

}

void skylight::EffectDriver::PublishLedBuffer() {
    mOnBuffer(&mBuffer);
}

void skylight::EffectDriver::UpdateLedBuffer() {
    mOnUpdate();
}

void skylight::EffectDriver::BlackNow() {
    spdlog::info("skylight effect driver setting all leds to black");

    std::fill(std::begin(mBuffer.buffer), std::begin(mBuffer.buffer) + mLedsPerStrip * mChannelCount * 3, 0);

    PublishLedBuffer();
    WaitUntilNextFrame();
    UpdateLedBuffer();
}

void skylight::EffectDriver::WaitUntilNextFrame() {
    std::this_thread::sleep_until(mNextFrameTimestamp - std::chrono::milliseconds(1)); // sleepy wait
    while (std::chrono::steady_clock::now() < mNextFrameTimestamp); // busy wait
    mNextFrameTimestamp = std::chrono::steady_clock::now() + mDelay;
}