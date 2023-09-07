#include "gpio.h"
#include "spdlog/spdlog.h"
#include "pigpio.h"
#include "skylight_message/trigger.hpp"


skylight::SPI::~SPI() {
    spiClose(mSpiDevice);
}

bool skylight::SPI::Connect(int chan, int speed, int flags) {
    mSpiDevice = spiOpen(chan, speed, flags);
    return mSpiDevice >= 0;
}


void skylight::SPI::SendBufferToHardware(const skylight_message::pixel_buffer *pMsg) {

    char *channelBuff = const_cast<char *>(reinterpret_cast<const char *>(pMsg->enabledChannels));

    if (strcmp(channelBuff, mEnabledChannels) != 0) {
        memcpy(mEnabledChannels, channelBuff, 24);
        int spiWriteByteCount = spiWrite(mSpiDevice, mEnabledChannels, 24);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    char *charBuffer = const_cast<char *>(reinterpret_cast<const char *>(pMsg->buffer));

    int spiWriteByteCount = spiWrite(mSpiDevice, charBuffer, 28800);

}


skylight::GPIO::~GPIO() {
    gpioTerminate();
}


bool skylight::GPIO::Connect() {
    if (!mMessaging.good()) {
        spdlog::error("led driver system failed to instantiate messaging correctly");
        return false;
    }

    if (gpioInitialise() < 0) {
        spdlog::error("GPIO system failed to initialise pigpio");
        return false;
    }

    mpConfig = skylight::GetConfig("skylight_gpio.toml");

    if (!mpConfig) {
        spdlog::error("led driver system failed to load config");
        return false;
    }

    auto [speedOK, speed] = mpConfig->getInt("speed");
    auto [spiDeviceOK, spiDevice] = mpConfig->getInt("spiDevice");

    if (!speedOK || !spiDeviceOK) {
        spdlog::error("led driver system failed to get the spi speed or spiDevice from the config");
        return false;
    }


    if (!mSPI.Connect(static_cast<int>(spiDevice), static_cast<int>(speed))) {
        spdlog::error("led driver failed to connect to the SPI device");
        return false;
    }

    lcm::Subscription *sub = mMessaging.subscribe("gpio/led_buffer", &GPIO::ReceiveBuffer, this);
    sub->setQueueCapacity(1);

    std::vector<long int> pGPIO_ports = *mpConfig->getArray("GPIO_ports")->getIntVector();
    auto [debounceOk, debounce] = mpConfig->getInt("debounce");


    if (!debounceOk) {
        spdlog::error("button system cannot load the debounce value");
        return false;
    }

    if (pGPIO_ports.empty()) {
        spdlog::error("button system doesn't have any configured GPIO ports");
        return false;
    }

    for (long int GPIO_port: pGPIO_ports) {
        spdlog::info("button system setting up gpio port {} with debounce of {}", GPIO_port, debounce);
        gpioSetMode(GPIO_port, PI_INPUT);
        gpioSetPullUpDown(GPIO_port, PI_PUD_UP);
        gpioGlitchFilter(GPIO_port, debounce);
        gpioSetAlertFuncEx(GPIO_port, &skylight::GPIO::ButtonCallback, static_cast<void *>(&mMessaging));
    }

    return true;

}


void skylight::GPIO::ButtonCallback(int gpioPin, int level, unsigned int tick, void *messaging) {
    spdlog::info("button {} state changed to {}", gpioPin, level);
    skylight_message::trigger button_press;
    std::string channel = fmt::format("gpio/button_{}_{}", gpioPin, level ? "up" : "down");
    static_cast<skylight::Messaging *>(messaging)->publish(channel, &button_press);
}

bool skylight::GPIO::Start() {
    mMessaging.Start();
    return true;
}

void skylight::GPIO::ReceiveBuffer(const skylight_message::pixel_buffer *pMsg) {
    mSPI.SendBufferToHardware(pMsg);
}

void skylight::GPIO::ReceiveBuffer(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                   const skylight_message::pixel_buffer *pMsg) {
    mSPI.SendBufferToHardware(pMsg);
}
