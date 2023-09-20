#pragma once

#include <functional>
#include <pigpio.h>
#include "skylight_messaging.h"
#include "skylight_message/pixel_buffer.hpp"
#include "skylight_message/simple_void.hpp"

namespace skylight {

    class SPI {
    public:
        SPI() = default;

        ~SPI();

        bool Connect(int chan, int speed, int flags = 0);

        bool SendBufferToHardware(const skylight_message::pixel_buffer *msg);

        bool SendUpdateCommand();

    private:
        int mSpiDevice;
        char mEnabledChannels[24];
        std::thread mSendThread;
    };


    class GPIO {
    public:
        GPIO(gpioAlertFuncEx_t buttonCallback, void *buttonCallbackContext);

        ~GPIO();

        void ReceiveBuffer(const skylight_message::pixel_buffer *msg);

        void Update();

    private:


        void Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                    const skylight_message::simple_void *msg);

        std::shared_ptr<toml::Table> mpConfig;
        SPI mSPI;
    };

}