#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <pigpio.h>
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"
#include "skylight_config.h"
#include "skylight_time.h"

#include "skylight_message/pixel_buffer.hpp"
#include "skylight_message/trigger.hpp"


class EffectDriver
{
public:
    EffectDriver() = default;

    bool Connect()
    {
        if (!mMessaging.good())
        {
            spdlog::error("led driver system failed to instantiate messaging correctly");
            return false;
        }

        std::shared_ptr<toml::Table> pConfig = skylight::GetConfig("skylight_effect_driver.toml");

        if(!pConfig)
        {
            spdlog::error("led driver system failed to load config");
            return false;
        }

        lcm::Subscription* sub = mMessaging.subscribe("effect_driver/update", &EffectDriver::Update, this);

        return true;
    }

    void Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::trigger *msg)
    {
        spdlog::info("update");
        mBuffer.timestamp = skylight::Now();
        mMessaging.publish("led/buffer", &mBuffer);
    }

    void Run()
    {
        while (0 == mMessaging.handle());
    }

private:
    skylight::Messaging mMessaging;
    std::shared_ptr<toml::Table> mPConfig;
    skylight_message::pixel_buffer mBuffer;

};


int main(int argc, char **argv) {


    EffectDriver effectDriver;

    effectDriver.Connect();

    effectDriver.Run();

    return 0;
}