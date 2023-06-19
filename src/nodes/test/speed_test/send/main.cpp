#include <iostream>
#include "spdlog/spdlog.h"
#include "skylight_message/pixel_buffer.hpp"
#include "skylight_messaging.h"
#include "skylight_time.h"
#include "CLI11.hpp"
#include <thread>


int main(int argc, char **argv) {

    CLI::App app{"speed test send with sleep"};

    std::string broadcastChannel = "pixel_buffer";
    app.add_option("-c,--channel", broadcastChannel, "The channel to send the pixelbuffer");

    CLI11_PARSE(app, argc, argv);

    skylight::Messaging messaging;
    if (!messaging.good())
        return 1;

    while (true)
    {
        skylight_message::pixel_buffer pixelBuffer;
        pixelBuffer.timestamp = skylight::Now();
        pixelBuffer.buffer[3999][0] = 69;
        messaging.publish(broadcastChannel, &pixelBuffer);
        spdlog::info("channel, {},sent,{}, last, {}", broadcastChannel, pixelBuffer.timestamp, pixelBuffer.buffer[3999][0]);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}