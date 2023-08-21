#include <thread>
#include <pigpio.h>
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"
#include "skylight_config.h"
#include "skylight_time.h"

#include "skylight_message/pixel_buffer.hpp"
#include "skylight_message/trigger.hpp"


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
        spdlog::info("buffer received on {} at {}", chan, msg->timestamp);

        char* charBuffer = const_cast<char*>(reinterpret_cast<const char*>(msg->buffer));

        spdlog::info("buffer sending");

        spiWrite(mSpiDevice, charBuffer, 28800);

        spdlog::info("buffer sent");

    }

private:
    int mSpiDevice;

};


void ButtonCallback(int gpioPin, int level, unsigned int tick, void* messaging)
{
    spdlog::info("button {} state changed to {}", gpioPin, level);
    skylight_message::trigger button_press;
    button_press.timestamp = skylight::Now();
    std::string channel =  fmt::format("button_{}_{}", gpioPin, level?"up":"down");
    static_cast<skylight::Messaging*>(messaging)->publish(channel, &button_press);
}


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

    std::shared_ptr<toml::Table> pConfig = skylight::GetConfig("skylight_gpio.toml");

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


    std::vector<long int> pGPIO_ports = *pConfig->getArray("GPIO_ports")->getIntVector();
    auto [debounceOk, debounce] = pConfig->getInt("debounce");


    if(!debounceOk)
    {
        spdlog::error("button system cannot load the debounce value");
        return 1;
    }
    if(pGPIO_ports.empty())
    {
        spdlog::error("button system doesn't have any configured GPIO ports");
        return 1;
    }

    for(long int GPIO_port: pGPIO_ports)
    {
        spdlog::info("button system setting up gpio port {} with debounce of {}", GPIO_port, debounce);
        gpioSetMode(GPIO_port, PI_INPUT);
        gpioSetPullUpDown(GPIO_port, PI_PUD_UP);
        gpioGlitchFilter(GPIO_port, debounce);
        gpioSetAlertFuncEx(GPIO_port, &ButtonCallback, static_cast<void*>(&messaging));
    }

    while (0 == messaging.handle());

    gpioTerminate();

    return 0;
}