#include "effect_driver_node.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {
    spdlog::info("this is the effect driver main");
    skylight::EffectDriverNode effectDriverNode;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}