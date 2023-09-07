#pragma once

#include "skylight_messaging.h"
#include "skylight_message/pixel_buffer.hpp"

namespace skylight {

    class SPI {
    public:
        SPI() = default;

        ~SPI();

        bool Connect(int chan, int speed, int flags = 0);

        void SendBufferToHardware(const skylight_message::pixel_buffer *msg);

    private:
        int mSpiDevice;
        char mEnabledChannels[24];
    };


    class GPIO {
    public:
        GPIO() = default;

        ~GPIO();

        bool Connect();

        bool Start();

        void ReceiveBuffer(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                           const skylight_message::pixel_buffer *msg);

        void ReceiveBuffer(const skylight_message::pixel_buffer *msg);

        static void ButtonCallback(int gpioPin, int level, unsigned int tick, void *messaging);

    private:
        skylight::Messaging mMessaging;
        std::shared_ptr<toml::Table> mpConfig;
        SPI mSPI;
    };

}