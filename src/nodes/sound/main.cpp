#include <thread>
#include <chrono>
#include "sound_node.h"

int main(int argc, char **argv) {

    skylight::Config config = skylight::GetConfig("skylight_sound.toml");

    std::vector<std::string> nodeNames = config->keys();

    std::vector<std::unique_ptr<skylight::SoundNode>> nodes;

    for (auto const &nodeName: nodeNames) {
        nodes.emplace_back(std::make_unique<skylight::SoundNode>(nodeName));
    }


    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

}


