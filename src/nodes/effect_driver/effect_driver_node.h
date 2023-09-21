#pragma once

#include "skylight_messaging.h"
#include "effect_driver.h"

namespace skylight {
    class EffectDriverNode {
    public:
        EffectDriverNode(std::shared_ptr<skylight::EffectDriver> effectDriverOverride = nullptr);

        ~EffectDriverNode();
        
    private:
        std::shared_ptr<skylight::EffectDriver> mpEffectDriver;
        skylight::Messaging mMessaging;
    };
}
