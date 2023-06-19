#include "skylight_messaging.h"
#include "skylight_time.h"
#include "skylight_message/pixel_buffer.hpp"
#include "spdlog/spdlog.h"
#include <thread>
#include "CLI11.hpp"


class TimestampPrinter {
public:
    TimestampPrinter() = default;

    void Print(const lcm::ReceiveBuffer *rbuf,
                        const std::string &chan,
                        const skylight_message::pixel_buffer *msg) {
        spdlog::info("channel, {},sent,{}, rec,{}, last, {}", chan, msg->timestamp, skylight::Now(),
                     msg->buffer[3999][0]);
    }
};

int main(int argc, char **argv) {

    CLI::App app{"speed test reciever"};

    std::string subscribeChannel = "pixel_buffer";
    app.add_option("-c,--channel", subscribeChannel, "The channel to receive the pixelbuffer");

    CLI11_PARSE(app, argc, argv);

    TimestampPrinter tsp;

    skylight::Messaging messaging;
    if (!messaging.good())
        return 1;

    messaging.subscribe(subscribeChannel, &TimestampPrinter::Print, &tsp);
    while (0 == messaging.handle());

    return 0;
}