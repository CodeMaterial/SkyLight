#include <lcm/lcm-cpp.hpp>
#include "skylight_message/button_press.hpp"
#include "../common/messaging.h"

int main(int argc, char ** argv)
{
    skylight::Messaging lcm;
    if(!lcm.good())
        return 1;

    skylight_message::button_press button_press;
    button_press.uid = "left_hand";
    lcm.publish("button_press", &button_press);

    return 0;
}