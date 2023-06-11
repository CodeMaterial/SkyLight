#include <stdio.h>
#include <lcm/lcm-cpp.hpp>
#include "skylight_message/button_press.hpp"

class Handler
{
public:
    ~Handler() {}

    void handleMessage(const lcm::ReceiveBuffer* rbuf,
                       const std::string& chan,
                       const skylight_message::button_press* msg)
    {
        printf("Received message on channel \"%s\":\n", chan.c_str());
        printf("  timestamp   = %lld\n", (long long)msg->timestamp);
        printf("  uid         = '%s'\n", msg->uid.c_str());
    }
};

int main(int argc, char** argv)
{
    lcm::LCM lcm;
    if(!lcm.good())
        return 1;

    Handler handlerObject;
    lcm.subscribe("button", &Handler::handleMessage, &handlerObject);

    while(0 == lcm.handle());

    return 0;
}