#include "effect.h"
#include <spdlog/spdlog.h>

skylight::Effect::Effect(Timestamp start, Duration duration) {
    mStart = start;
    mDuration = duration;
}

void skylight::Effect::AddLEDS(std::vector<LED> leds) {
    mLeds = leds;
}

bool skylight::Effect::HasStarted(Timestamp t) {
    return t.time_since_epoch().count() > mStart.time_since_epoch().count();
}

bool skylight::Effect::HasFinished(Timestamp t) {
    return t.time_since_epoch().count() > (mStart + mDuration).time_since_epoch().count();
}

std::vector<skylight::LED> skylight::Effect::RenderTimestamp(Timestamp timestamp) {
    return Render(timestamp - mStart);
}