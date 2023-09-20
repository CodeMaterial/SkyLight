#pragma once

#include "skylight_messaging.h"
#include "skylight_message/pixel_buffer.hpp"
#include "skylight_message/simple_void.hpp"
#include "gpio.h"

namespace skylight {
    class GpioNode {
    public:
        GpioNode();

        ~GpioNode();

        static void OnButton(int gpioPin, int level, uint32_t tick, void *context);

        void Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                    const skylight_message::simple_void *msg);

        void ReceiveBuffer(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                           const skylight_message::pixel_buffer *msg);
        
    private:
        skylight::Messaging mMessaging;
        std::shared_ptr<skylight::GPIO> mpGpio;
    };
}