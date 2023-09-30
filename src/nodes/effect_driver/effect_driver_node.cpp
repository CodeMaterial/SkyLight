#include "effect_driver_node.h"
#include <exception>
#include <memory>
#include "effects/effects/basic_effect.h"

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

    mMessaging.subscribe("/effect_driver/effects/start", &EffectDriverNode::AddEffect, this);

    mMessaging.Start();
}

skylight::EffectDriverNode::~EffectDriverNode() {

}

void skylight::EffectDriverNode::Run() {
    mpEffectDriver->Run();
}

void skylight::EffectDriverNode::AddEffect(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                           const skylight_message::simple_string *msg) {

    std::string effectName = msg->data;

    if (effectName == "basic_effect") {
        Timestamp start = std::chrono::steady_clock::now();
        Duration duration = std::chrono::seconds(5);
        mpEffectDriver->AddEffect(std::make_shared<BasicEffect>(start, duration));
    }
}