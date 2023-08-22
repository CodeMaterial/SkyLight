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

        for(int i = 0; i < 24; i++)
        {
            mBuffer.enabledChannels[i] = 1;
        }


        return true;
    }

    void Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan, const skylight_message::trigger *msg)
    {
        spdlog::info("update");
        mBuffer.timestamp = skylight::Now();
        int hz = 50;
        std::chrono::time_point start = std::chrono::system_clock::now();
        for(int i = 0; i < hz; i++)
        {
            std::this_thread::sleep_until(start + std::chrono::microseconds((1000000/hz)*i));
            spdlog::info("sending buffer");
            mMessaging.publish("led/buffer", &mBuffer);

            if(mBuffer.enabledChannels[0] == 0) // lets flick the first channel on and off
                mBuffer.enabledChannels[0] = 1;
            else
                mBuffer.enabledChannels[0] = 0;
        }
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