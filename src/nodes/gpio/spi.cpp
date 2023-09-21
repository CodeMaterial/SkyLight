#include "spi.h"
#include "pigpio.h"
#include <thread>
#include <chrono>
#include "spdlog/spdlog.h"

skylight::SPI::SPI(int chan, int speed, int flags) {
    mSpiDevice = spiOpen(chan, speed, flags);
    if (mSpiDevice < 0) {
        throw std::runtime_error(
                fmt::format("skylight spi failed to connect to SPI device, {} {} {}", chan, speed, flags));
    }
}

skylight::SPI::~SPI() {
    spiClose(mSpiDevice);
}

bool skylight::SPI::SendBufferToHardware(const skylight_message::pixel_buffer *pMsg) {
    // these values need to be not-hard coded, but they're not, too bad!

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

    // this is to ensure that the chip select line gets pulled down for long enough before the next command
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    return spiWriteByteCount == 28800;

}

bool skylight::SPI::SendUpdateCommand() {
    char command = '1';
    char *charBuffer = &command;
    int spiWriteByteCount = spiWrite(mSpiDevice, charBuffer, 1); // hu "error -1 decoding trigger!!!"

    // this is to ensure that the chip select line gets pulled down for long enough before the next command
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    return spiWriteByteCount == 1;
}