#include "compositor.h"
#include "harness.h"
#include <spdlog/spdlog.h>

skylight::Compositor::Compositor() {
    InitLeds();
}

skylight::Compositor::~Compositor() {

}

void skylight::Compositor::InitLeds() {
    // this is where the heavy lifting comes in when it comes to setting led positions and
    // neighbours
    for (int ledIndex = 0; ledIndex < 24 * 400; ledIndex++) {
        mLeds.push_back({ledIndex, {}, {}});
    }
}

void skylight::Compositor::AddEffect(std::shared_ptr<Effect> e) {
    e->AddLEDS(mLeds);
    mEffectQueue.push_back(e);
}

void skylight::Compositor::Render(Timestamp timestamp) {

    // lets get rid of any effects that have been completed
    mEffectQueue.erase(std::remove_if(mEffectQueue.begin(), mEffectQueue.end(),
                                      [timestamp](std::shared_ptr<Effect> e) { return e->HasFinished(timestamp); }),
                       mEffectQueue.end());

    for (std::shared_ptr<Effect> e: mEffectQueue) {
        if (e->HasStarted(timestamp) && !e->HasFinished(timestamp)) {
            auto f = std::async(&skylight::Effect::RenderTimestamp, e, timestamp);
            mEffectBuffersFuture.emplace_back(std::move(f));
        }
    }
}

void skylight::Compositor::Compose(uint8_t *buffer) {

    memset(buffer, 0, 400 * 24 * 3);

    for (std::future<std::vector<LED>> &leds: mEffectBuffersFuture) {
        for (LED const &led: leds.get()) {
            buffer[led.bufferIndex * 3 + 0] =
                    static_cast<uint8_t >((1.0f - led.rgba.a) * buffer[led.bufferIndex * 3] + led.rgba.a * led.rgba.r);
            buffer[led.bufferIndex * 3 + 1] =
                    static_cast<uint8_t >((1.0f - led.rgba.a) * buffer[led.bufferIndex * 3] + led.rgba.a * led.rgba.g);
            buffer[led.bufferIndex * 3 + 2] =
                    static_cast<uint8_t >((1.0f - led.rgba.a) * buffer[led.bufferIndex * 3] + led.rgba.a * led.rgba.b);
        }
    }
    mEffectBuffersFuture.clear();
}