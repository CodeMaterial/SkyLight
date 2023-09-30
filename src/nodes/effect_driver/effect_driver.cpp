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

void skylight::EffectDriver::Run() {
    mNextFrameTimestamp = std::chrono::steady_clock::now() + mDelay; // set first timestamp in the future
    mCompositor.Render(mNextFrameTimestamp); //  start rendering that frame

    while (true) {
        mCompositor.Compose(mBuffer.buffer); // compose the frame once rendering is complete
        mCompositor.Render(mNextFrameTimestamp + mDelay); // start rendering the next frame
        PublishLedBuffer(); // send led data out to hardware
        WaitUntilNextFrame(); // wait until the next frame is avaliable
        UpdateLedBuffer(); // send the update tick
    }
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

    auto now = std::chrono::steady_clock::now();
    if (now > mNextFrameTimestamp) {
        mNextFrameTimestamp = now + mDelay;
    } else {
        std::this_thread::sleep_until(mNextFrameTimestamp - std::chrono::milliseconds(1)); // sleepy wait
        while (std::chrono::steady_clock::now() < mNextFrameTimestamp); // busy wait
        mNextFrameTimestamp += mDelay;
    }
}

void skylight::EffectDriver::AddEffect(std::shared_ptr<skylight::Effect> e) {
    mCompositor.AddEffect(e);
}