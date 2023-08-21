#include "spdlog/spdlog.h"
#include "CLI11.hpp"
#include "skylight_messaging.h"
#include "skylight_config.h"

#include "skylight_message/pixel_buffer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <pigpio.h>


class BufferHandler
{
public:
    BufferHandler() = default;

    ~BufferHandler()
    {
        spiClose(mSpiDevice);
    }

    bool Connect(int chan, int speed, int flags=0)
    {
        mSpiDevice = spiOpen(chan, speed, flags);
        if (mSpiDevice < 0)
        {
            spdlog::error("failed to connect to spi channel {} at speed {}", chan, speed);
            return false;
        }

        return true;
    }


    void LCMBufferUpdate(const lcm::ReceiveBuffer *rbuf,
                     const std::string &chan,
                     const skylight_message::pixel_buffer *msg) {
        spdlog::info("buffer recieved on {} at {}", chan, msg->timestamp);

        // lets try and turn a const uint8_t* into a char*

        char* charBuffer = const_cast<char*>(reinterpret_cast<const char*>(msg->buffer));

        spiWrite(mSpiDevice, charBuffer, 28800);
    }

private:
    int mSpiDevice;

};

int main(int argc, char **argv) {

    CLI::App app{"Skylight led driver system"};

    CLI11_PARSE(app, argc, argv);

    skylight::Messaging messaging;
    if (!messaging.good())
    {
        spdlog::info("skylight messaging failed to instantiate correctly");
        return 1;
    }

    if (gpioInitialise() < 0) return 1;

    std::shared_ptr<toml::Table> pConfig = skylight::GetConfig("skylight_led_driver.toml");

    if(!pConfig)
    {
        return 1;
    }

    auto [speedOK, speed] = pConfig->getInt("speed");

    if(!speedOK)
    {
        return 1;
    }

    BufferHandler bufferHandler;
    if(!bufferHandler.Connect(0, speed))
    {
        return 1;
    }

    lcm::Subscription* sub = messaging.subscribe("led/buffer", &BufferHandler::LCMBufferUpdate, &bufferHandler);
    sub->setQueueCapacity(1);

    while (0 == messaging.handle());

    gpioTerminate();

    return 0;


}