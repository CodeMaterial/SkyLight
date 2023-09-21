#include "gpio_node.h"

int main(int argc, char **argv) {

    spdlog::info("skylight gpio node starting");
    skylight::GpioNode gpioNode;
    spdlog::info("skylight gpio node started");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}