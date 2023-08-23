#include <chrono>
#include <thread>
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"
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

        mMessaging.subscribe("effect_driver/update", &EffectDriver::Update, this);

        std::fill(std::begin(mBuffer.buffer),std::begin(mBuffer.buffer)+28800,0);

        std::fill(std::begin(mBuffer.enabledChannels),std::begin(mBuffer.enabledChannels)+24,1);

        return true;
    }

    void Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::trigger *msg)
    {
        spdlog::info("update");
        mBuffer.timestamp = skylight::Now();
        int hz = 50;

        for(int channelID=0; channelID < 24; channelID++)
        {
            mBuffer.buffer[channelID*400*3 + channelID] = 255; // c0 = 1:red, c1 = 1:green, c2 = 1:blue, c3 = 2:red ...
        }

        if(mBuffer.enabledChannels[0] == 1)
            mBuffer.enabledChannels[0] = 0;
        else
            mBuffer.enabledChannels[0] = 1;

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
    int temp = 0;
};


int main(int argc, char **argv) {


    EffectDriver effectDriver;

    effectDriver.Connect();

    effectDriver.Run();

    return 0;
}