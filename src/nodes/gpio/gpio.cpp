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


bool skylight::SPI::SendBufferToHardware(const skylight_message::pixel_buffer *pMsg) {

    char *channelBuff = const_cast<char *>(reinterpret_cast<const char *>(pMsg->enabledChannels));

    if (strcmp(channelBuff, mEnabledChannels) != 0) {
        memcpy(mEnabledChannels, channelBuff, 24);
        int spiWriteByteCount = spiWrite(mSpiDevice, mEnabledChannels, 24);
        if (spiWriteByteCount != 24)
            return false;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    char *charBuffer = const_cast<char *>(reinterpret_cast<const char *>(pMsg->buffer));

    int spiWriteByteCount = spiWrite(mSpiDevice, charBuffer, 28800);

    if (spiWriteByteCount != 28800)
        return false;

    std::this_thread::sleep_for(std::chrono::microseconds(10));

    spdlog::info("spi system finished sending buffer to hardware");

    return true;

}

// this threaded version is //very// broken, needs fixing
bool skylight::SPI::SendBufferToHardwareAsync(const skylight_message::pixel_buffer *pMsg) {

    spdlog::info("waiting for send thread to be joinable");
    if (mSendThread.joinable()) mSendThread.join();
    spdlog::info("spinning up new thread");

    //this is hacky and needs fixing
    mSendThread = std::thread([this, pMsg]() {
        skylight_message::pixel_buffer msgCopy;
        msgCopy.timestamp = pMsg->timestamp;
        std::copy(std::begin(pMsg->buffer), std::end(pMsg->buffer), std::begin(msgCopy.buffer));
        std::copy(std::begin(pMsg->enabledChannels), std::end(pMsg->enabledChannels),
                  std::begin(msgCopy.enabledChannels));
        SendBufferToHardware(&msgCopy);
    });
}


bool skylight::SPI::SendUpdateCommand() {
    char command = '1';
    char *charBuffer = &command;
    int spiWriteByteCount = spiWrite(mSpiDevice, charBuffer, 1); // hu "error -1 decoding trigger!!!"
    if (spiWriteByteCount != 1)
        return false;

    std::this_thread::sleep_for(std::chrono::microseconds(10));
    return true;
}

skylight::GPIO::GPIO() {

    spdlog::info("gpio system initialising");

    if (gpioInitialise() < 0) {
        throw std::runtime_error("gpio system failed to initialise pigpio");
    }

    mpConfig = skylight::GetConfig("skylight_gpio.toml");

    auto [speedOK, speed] = mpConfig->getInt("speed");
    auto [spiDeviceOK, spiDevice] = mpConfig->getInt("spiDevice");

    if (!speedOK || !spiDeviceOK) {
        throw std::runtime_error("gpio system failed to load speed or device from config");
    }

    if (!mSPI.Connect(static_cast<int>(spiDevice), static_cast<int>(speed))) {
        throw std::runtime_error("gpio system failed to connect to the SPI device");
    }

    std::vector<long int> pGPIO_ports = *mpConfig->getArray("GPIO_ports")->getIntVector();
    auto [debounceOk, debounce] = mpConfig->getInt("debounce");

    if (!debounceOk) {
        throw std::runtime_error("gpio system failed to load debounce value from config");
    }

    if (pGPIO_ports.empty()) {
        spdlog::warn("gpio system failed to load pin numbers from config, assuming no buttons registered");
    }

    for (long int GPIO_port: pGPIO_ports) {
        spdlog::info("button system setting up gpio port {} with debounce of {}", GPIO_port, debounce);
        gpioSetMode(GPIO_port, PI_INPUT);
        gpioSetPullUpDown(GPIO_port, PI_PUD_UP);
        gpioGlitchFilter(GPIO_port, debounce);
        gpioSetAlertFuncEx(GPIO_port, &skylight::GPIO::ButtonCallback, static_cast<void *>(&mMessaging));
    }


    if (!mMessaging.good()) {
        throw std::runtime_error("gpio system failed to load messaging");
    }

    mMessaging.subscribe("gpio/update", &GPIO::Update, this);
    lcm::Subscription *sub = mMessaging.subscribe("gpio/led_buffer", &GPIO::ReceiveBuffer, this);
    sub->setQueueCapacity(1);


    spdlog::info("gpio system initialised");
}

skylight::GPIO::~GPIO() {
    gpioTerminate();
}


void skylight::GPIO::ButtonCallback(int gpioPin, int level, unsigned int tick, void *messaging) {
    spdlog::info("button {} state changed to {}", gpioPin, level);
    skylight_message::trigger button_press;
    std::string channel = fmt::format("gpio/button_{}_{}", gpioPin, level ? "up" : "down");
    static_cast<skylight::Messaging *>(messaging)->publish(channel, &button_press);
}

void skylight::GPIO::Start() {
    spdlog::info("gpio system starting");
    mMessaging.Start();
}


void skylight::GPIO::Update() {
    spdlog::info("gpio system sending update to SPI hardware");
    if (!mSPI.SendUpdateCommand()) {
        spdlog::error("gpio system failed to send update command");
    }
}

void skylight::GPIO::Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                            const skylight_message::trigger *msg) {
    spdlog::info("gpio system received update command");
    Update();
}

void skylight::GPIO::ReceiveBuffer(const skylight_message::pixel_buffer *pMsg) {
    spdlog::info("gpio system received buffer");

    mSPI.SendBufferToHardware(pMsg);

}

void skylight::GPIO::ReceiveBuffer(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                   const skylight_message::pixel_buffer *pMsg) {
    ReceiveBuffer(pMsg);
}
