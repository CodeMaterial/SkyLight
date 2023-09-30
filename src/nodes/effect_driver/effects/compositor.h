#pragma once

#include <vector>
#include "effect.h"
#include <future>


namespace skylight {

    class Compositor {
    public:
        Compositor();

        ~Compositor();

        void InitLeds();

        void AddEffect(std::shared_ptr<skylight::Effect> e);

        void Render(Timestamp timePoint);

        void Compose(uint8_t *buffer);

    private:

        std::vector<LED> mLeds;
        std::vector<std::shared_ptr<skylight::Effect>> mEffectQueue;
        std::vector<std::future<std::vector<skylight::LED>>> mEffectBuffersFuture;

    };
}