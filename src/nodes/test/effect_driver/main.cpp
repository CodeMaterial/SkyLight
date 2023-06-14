#include "skylight_messaging.h"
#include "skylight_message/user_command.hpp"
#include "spdlog/spdlog.h"

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
    }
};

int main(int argc, char **argv) {
    skylight::Messaging messaging;
    if (!messaging.good())
        return 1;

    EffectDriver effectDriver;
    messaging.subscribe("effects/start", &EffectDriver::StartEffect, &effectDriver);

    while (0 == messaging.handle());

    return 0;
}