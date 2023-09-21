#include "speech_node.h"

int main(int argc, char *argv[]) {

    spdlog::info("skylight speech node starting");

    skylight::SpeechNode speechNode;

    spdlog::info("skylight speech node started");

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}