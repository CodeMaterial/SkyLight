#pragma once

#include <chrono>
#include "../effect.h"
#include "../harness.h"
#include "../../hsv_to_rgb.h"

#include <spdlog/spdlog.h>

namespace skylight {

    class BasicEffect : public Effect {
    public:
        BasicEffect(Timestamp start, Duration duration) : Effect(start, duration) {
        };

        std::vector<LED> Render(Duration timeSinceStart) override {


            float effectProgress = static_cast<float>(timeSinceStart.count()) / static_cast<float>(mDuration.count());
            HSV hsv{255, 255, 255};
            RGB rgb{0, 0, 0};

            for (int i = 0; i < 400; i++) {

                hsv.hue = static_cast<uint8_t>(255.0 * ((static_cast<float>(i) / 200.0f) + effectProgress));
                hsv2rgb_rainbow(hsv, rgb);

                mLeds[i].rgba = {rgb.r, rgb.g, rgb.b, 1.0};
                mLeds[400 - i].rgba = {rgb.r, rgb.g, rgb.b, 1.0};

            }
            return mLeds;
        };
    private:

    };

}