#pragma once

#include <chrono>
#include "harness.h"

typedef std::chrono::time_point<std::chrono::steady_clock> Timestamp;
typedef std::chrono::duration<long, std::ratio<1, 1000000000>> Duration;


namespace skylight {
    class Effect {
    public:
        Effect(Timestamp start, Duration duration);

        ~Effect() = default;

        void AddLEDS(std::vector<LED> leds);

        bool HasStarted(Timestamp t);

        Timestamp GetStartTime();

        bool HasFinished(Timestamp t);

        std::vector<LED> RenderTimestamp(Timestamp timestamp);

        virtual std::vector<LED> Render(Duration timeSinceStart) = 0;

    protected:
        std::vector<LED> mLeds;
        Timestamp mStart;
        Duration mDuration;
    };
}