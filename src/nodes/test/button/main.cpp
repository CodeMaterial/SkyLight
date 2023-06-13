#include "../../../common/messaging.h"
#include "skylight_message/user_command.hpp"

int main(int argc, char ** argv)
{
    skylight::Messaging messaging;
    if(!messaging.good())
        return 1;

    skylight_message::user_command button_press;
    messaging.publish("button_press", &button_press);

    return 0;
}