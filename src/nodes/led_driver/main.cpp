#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <pigpio.h>
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"
#include "skylight_config.h"

#include "skylight_message/pixel_buffer.hpp"


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
        return mSpiDevice >= 0;
    }


    void LCMBufferUpdate(const lcm::ReceiveBuffer *rbuf,
                     const std::string &chan,
                     const skylight_message::pixel_buffer *msg) {
        spdlog::debug("buffer received on {} at {}", chan, msg->timestamp);

        char* charBuffer = const_cast<char*>(reinterpret_cast<const char*>(msg->buffer));

        spiWrite(mSpiDevice, charBuffer, 28800);
    }

private:
    int mSpiDevice;

};

int main(int argc, char **argv) {

    skylight::Messaging messaging;
    if (!messaging.good())
    {
        spdlog::error("led driver system failed to instantiate messaging correctly");
        return 1;
    }

    if (gpioInitialise() < 0)
    {
        spdlog::error("led driver system failed to initialise the gpio system");
        return 1;
    }

    std::shared_ptr<toml::Table> pConfig = skylight::GetConfig("skylight_led_driver.toml");

    if(!pConfig)
    {
        spdlog::error("led driver system failed to load config");
        return 1;
    }

    auto [speedOK, speed] = pConfig->getInt("speed");
    auto [spiDeviceOK, spiDevice] = pConfig->getInt("spiDevice");

    if(!speedOK || !spiDeviceOK)
    {
        spdlog::error("led driver system failed to get the spi speed or spiDevice from the config");
        return 1;
    }

    BufferHandler bufferHandler{};
    if(!bufferHandler.Connect(static_cast<int>(spiDevice), static_cast<int>(speed)))
    {
        spdlog::error("led driver failed to connect to the SPI device");
        return 1;
    }

    lcm::Subscription* sub = messaging.subscribe("led/buffer", &BufferHandler::LCMBufferUpdate, &bufferHandler);
    sub->setQueueCapacity(1);

    while (0 == messaging.handle());

    gpioTerminate();

    return 0;
}