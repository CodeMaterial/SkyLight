#include "skylight_messaging.h"
#include "skylight_message/user_command.hpp"
#include "spdlog/spdlog.h"
#include <thread>

class EffectDriver {
public:
    EffectDriver() = default;

    ~EffectDriver() = default;

    void StartEffect(const lcm::ReceiveBuffer *rbuf,
                     const std::string &chan,
                     const skylight_message::user_command *msg) {
        spdlog::info("Received message on channel {}", chan.c_str());
        spdlog::info("timestamp = {}", msg->timestamp);
        spdlog::info("command = {}", msg->command.c_str());
        spdlog::info("sleeping for 1 second");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        spdlog::info("sleeping complete");

    }
};

int main(int argc, char **argv) {
    skylight::Messaging messaging;
    if (!messaging.good())
        return 1;

    EffectDriver effectDriver;
    lcm::Subscription* sub =
            messaging.subscribe("effects/start", &EffectDriver::StartEffect, &effectDriver);
    sub->setQueueCapacity(30);
    sub->getQueueSize();
    while (0 == messaging.handle());

    return 0;
}