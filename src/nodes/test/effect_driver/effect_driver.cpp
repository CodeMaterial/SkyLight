#include <cstdio>
#include "../../../common/messaging.h"
#include "skylight_message/user_command.hpp"

class Handler
{
public:
    Handler() {}
    ~Handler() {}

    void handleMessage(const lcm::ReceiveBuffer* rbuf,
                       const std::string& chan,
                       const skylight_message::user_command* msg)
    {
        printf("Received message on channel \"%s\":\n", chan.c_str());
        printf("  timestamp   = %lld\n", (long long)msg->timestamp);
        printf("  uid         = '%s'\n", msg->command.c_str());
    }
};

int main(int argc, char** argv)
{
    skylight::Messaging messaging;
    if(!messaging.good())
        return 1;

    Handler handlerObject;
    messaging.subscribe("effects/stop", &Handler::handleMessage, &handlerObject);

    while(0 == messaging.handle());

    return 0;
}