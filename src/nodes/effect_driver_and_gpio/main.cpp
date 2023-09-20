#include "../effect_driver/effect_driver.h"
#include "../effect_driver/effect_driver_node.h"
#include "../gpio/gpio_node.h"
#include "spdlog/spdlog.h"

int main(int argc, char **argv) {
    spdlog::info("this is the combined one");

    skylight::GpioNode gpioNode;

    auto onBuffer = [&gpioNode](const skylight_message::pixel_buffer *pBuffer) {
        gpioNode.ReceiveBuffer(nullptr, "", pBuffer);
    };

    auto onUpdate = [&gpioNode]() {
        skylight_message::simple_void trigger;
        gpioNode.Update(nullptr, "", &trigger);
    };

    auto pEffectDriver = std::make_shared<skylight::EffectDriver>(onBuffer, onUpdate);

    skylight::EffectDriverNode effectDriverNode(pEffectDriver);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


}