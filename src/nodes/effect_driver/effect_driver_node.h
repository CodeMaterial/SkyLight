#pragma once

#include "skylight_messaging.h"
#include "effect_driver.h"
#include "skylight_message/simple_string.hpp"

namespace skylight {
    class EffectDriverNode {
    public:
        EffectDriverNode(std::shared_ptr<skylight::EffectDriver> effectDriverOverride = nullptr);

        ~EffectDriverNode();

        void
        AddEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::simple_string *msg);

        void Run();

    private:
        std::shared_ptr<skylight::EffectDriver> mpEffectDriver;
        skylight::Messaging mMessaging;
    };
}
