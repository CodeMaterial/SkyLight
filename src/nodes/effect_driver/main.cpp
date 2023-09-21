#include "effect_driver_node.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {

    spdlog::info("skylight effect driver node starting");
    skylight::EffectDriverNode effectDriverNode;
    spdlog::info("skylight effect driver node started");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}