#include "gpio.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {
    spdlog::info("this is the gpio main");
    skylight::GPIO gpio;
    gpio.Start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}