#pragma once

#include "skylight_message/pixel_buffer.hpp"

namespace skylight {
    class SPI {
    public:
        SPI(int chan, int speed, int flags = 0);

        ~SPI();

        bool SendBufferToHardware(const skylight_message::pixel_buffer *msg);

        bool SendUpdateCommand();

    private:
        int mSpiDevice;
        char mEnabledChannels[24];
    };
}