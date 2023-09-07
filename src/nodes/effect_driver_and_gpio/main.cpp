#include "../effect_driver/effect_driver.h"
#include "../gpio/gpio.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {
    spdlog::info("this is the combined one");

    auto gpio = std::make_shared<skylight::GPIO>();
    auto effectDriver = std::make_shared<skylight::EffectDriver>();

    auto bufferPublishOverride = [gpio](const skylight_message::pixel_buffer *pPixelBuffer) {
        gpio->ReceiveBuffer(pPixelBuffer);
    };

    effectDriver->RegisterBufferPublishOverride(bufferPublishOverride);

    gpio->Start();
    effectDriver->Start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


}