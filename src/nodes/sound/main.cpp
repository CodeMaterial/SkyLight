#include "spdlog/spdlog.h"
#include "CLI11.hpp"
#include "skylight_messaging.h"


int main(int argc, char **argv) {

    CLI::App app{"Skylight sound system"};

    CLI11_PARSE(app, argc, argv);

    skylight::Messaging messaging;
    if (!messaging.good())
    {
        spdlog::info("skylight messaging failed to instantiate correctly");
        return 1;
    }

    return 0;
}