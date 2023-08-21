#include <iostream>
#include <thread>
#include <fmt/core.h>

#include "spdlog/spdlog.h"
#include "skylight_message/user_command.hpp"
#include "skylight_messaging.h"
#include "skylight_time.h"
#include "skylight_config.h"


#include <pigpio.h>


void ButtonCallback(int gpioPin, int level, unsigned int tick, void* messaging)
{
    skylight_message::user_command button_press;
    button_press.timestamp = skylight::Now();
    std::string channel =  fmt::format("button_{}_{}", gpioPin, level?"up":"down");
    static_cast<skylight::Messaging*>(messaging)->publish(channel, &button_press);

}


int main(int argc, char **argv) {

    std::shared_ptr<toml::Table> pConfigTable = skylight::GetConfig("skylight_button.toml");

    if(!pConfigTable)
    {
        return 1;
    }

    std::vector<long int> pGPIO_ports = *pConfigTable->getArray("GPIO_ports")->getIntVector();
    auto [ok, debounce] = pConfigTable->getInt("debounce");


    if(!ok || pGPIO_ports.size() == 0)
    {
        return 1;
    }

    skylight::Messaging messaging;
    if (!messaging.good()) {
        return 1;
    }

    if(gpioInitialise() < 0)
    {
        return 1;
    }

    for(int GPIO_port: pGPIO_ports)
    {
        spdlog::info("setting up gpio port {} with debounce of {}", GPIO_port, debounce);
        gpioSetMode(GPIO_port, PI_INPUT);
        gpioSetPullUpDown(GPIO_port, PI_PUD_UP);
        gpioGlitchFilter(GPIO_port, debounce);
        gpioSetAlertFuncEx(GPIO_port, &ButtonCallback, static_cast<void*>(&messaging));
    }

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    return 0;
}