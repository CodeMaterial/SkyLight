#include <iostream>
#include <thread>
#include <fmt/core.h>

#include "spdlog/spdlog.h"
#include "skylight_message/user_command.hpp"
#include "skylight_messaging.h"
#include "skylight_time.h"


#include <pigpio.h>


void ButtonCallback(int gpioPin, int level, unsigned int tick, void* messaging)
{
    skylight_message::user_command button_press;
    button_press.timestamp = skylight::Now();
    std::string channel =  fmt::format("button_{}_{}", gpioPin, level?"up":"down");
    static_cast<skylight::Messaging*>(messaging)->publish(channel, &button_press);

}


int main(int argc, char **argv) {
    skylight::Messaging messaging;
    if (!messaging.good())
        return 1;

    int gpioA = 17;
    int gpioB = 27;

    gpioInitialise();

    gpioSetMode(gpioA, PI_INPUT);
    gpioSetMode(gpioB, PI_INPUT);

    /* pull up is needed as encoder common is grounded */

    gpioSetPullUpDown(gpioA, PI_PUD_UP);
    gpioSetPullUpDown(gpioB, PI_PUD_UP);

    gpioGlitchFilter(gpioA, 1000); //ms / 1 millisecond
    gpioGlitchFilter(gpioB, 1000); //ms / 1 millisecond

    /* monitor encoder level changes */

    gpioSetAlertFuncEx(gpioA, &ButtonCallback, static_cast<void*>(&messaging));
    gpioSetAlertFuncEx(gpioB, &ButtonCallback, static_cast<void*>(&messaging));

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    return 0;
}