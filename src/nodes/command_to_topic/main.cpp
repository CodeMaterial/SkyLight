#include "command_to_topic_node.h"
#include "skylight_message/simple_float.hpp"
#include "skylight_message/simple_string.hpp"

//I don't like this...
std::map<std::string, int> NUMBER_LOOKUP = {{"one",   1},
                                            {"two",   2},
                                            {"three", 3},
                                            {"four",  4},
                                            {"five",  5},
                                            {"six",   6},
                                            {"seven", 7},
                                            {"eight", 8},
                                            {"nine",  9},
                                            {"ten",   10}};

void SetVolume(std::vector<std::string> args, void *vpMessaging) {

    auto *pMessaging = static_cast<skylight::Messaging *>(vpMessaging);

    int volume;
    std::string audio_device;

    if (args.size() == 2) {
        audio_device = args[0];
        volume = NUMBER_LOOKUP[args[1]];
    } else {
        audio_device = "internal";
        volume = NUMBER_LOOKUP[args[0]];
    }

    skylight_message::simple_float volumeMsg{0, static_cast<float>(volume) / 10.0f};

    pMessaging->publish("/sound/" + audio_device + "/volume", &volumeMsg);
}

void WhatAreYou(std::vector<std::string> args, void *vpMessaging) {
    spdlog::info("calling callback");
    auto *pMessaging = static_cast<skylight::Messaging *>(vpMessaging);

    skylight_message::simple_string strMsg{0, "I am a pimoroni"};
    pMessaging->publish("/sound/internal/say", &strMsg);
}

int main(int argc, char **argv) {
    skylight::CommandToTopicNode cttn;

    cttn.RegisterOption("number", {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"});
    cttn.RegisterOption("audio_device", {"internal", "external"});
    cttn.RegisterCommand("set <audio_device> volume to <number>", SetVolume);
    cttn.RegisterCommand("set volume to <number>", SetVolume);

    cttn.RegisterCommand("what are you", WhatAreYou);

    cttn.Ready();

    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(1));

}


