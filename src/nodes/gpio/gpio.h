#pragma once

#include <pigpio.h>

#include "spi.h"
#include "skylight_messaging.h"
#include "skylight_message/pixel_buffer.hpp"
#include "skylight_message/simple_void.hpp"

namespace skylight {

    class GPIO {
    public:
        GPIO(gpioAlertFuncEx_t buttonCallback, void *buttonCallbackContext);

        ~GPIO();

        void ReceiveBuffer(const skylight_message::pixel_buffer *msg);

        void Update();

    private:

        std::shared_ptr<SPI> mpSPI;
    };

}