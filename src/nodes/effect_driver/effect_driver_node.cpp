#include "effect_driver_node.h"
#include <exception>

skylight::EffectDriverNode::EffectDriverNode(std::shared_ptr<skylight::EffectDriver> pEffectDriverOverride) {

    if (!mMessaging.good()) {
        throw std::runtime_error("skylight effect driver node failed to start the messaging system");
    }

    if (pEffectDriverOverride) {
        spdlog::info("skylight effect driver node has received an effect driver override");
        mpEffectDriver = pEffectDriverOverride;
    }


    if (!mpEffectDriver) {

        auto onBuffer = [this](const skylight_message::pixel_buffer *pBuffer) {
            mMessaging.publish("/effect_driver/buffer", pBuffer);
        };

        auto onUpdate = [this]() {
            skylight_message::simple_void simpleVoid;
            mMessaging.publish("/effect_driver/update", &simpleVoid);
        };

        mpEffectDriver = std::make_shared<skylight::EffectDriver>(onBuffer, onUpdate);
    }

    mMessaging.subscribe("/effect_driver/effects/test/start", &EffectDriver::TestEffect, mpEffectDriver.get());

    mMessaging.Start();
}

skylight::EffectDriverNode::~EffectDriverNode() {

}