#include "gpio_node.h"
#include "gpio.h"


skylight::GpioNode::GpioNode() {

    if (!mMessaging.good()) {
        throw std::runtime_error("gpio node failed to start messaging correctly");
    }

    mpGpio = std::make_shared<skylight::GPIO>(skylight::GpioNode::OnButton, this);


    mMessaging.subscribe("gpio/update", &GpioNode::Update, this);
    mMessaging.subscribe("gpio/led_buffer", &GpioNode::ReceiveBuffer, this)->setQueueCapacity(1);

    mMessaging.Start();
}

skylight::GpioNode::~GpioNode() {

}

void skylight::GpioNode::Update(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                const skylight_message::simple_void *msg) {
    mpGpio->Update();
}

void skylight::GpioNode::ReceiveBuffer(const lcm::ReceiveBuffer *rbuf, const std::string &chan,
                                       const skylight_message::pixel_buffer *msg) {
    mpGpio->ReceiveBuffer(msg);
}

void skylight::GpioNode::OnButton(int gpioPin, int level, uint32_t tick, void *context) {

    skylight::GpioNode *pGpioNode = static_cast<skylight::GpioNode *>(context);

    spdlog::info("button {} state changed to {}", gpioPin, level);
    skylight_message::simple_void button_press;
    std::string channel = fmt::format("gpio/button_{}/{}", gpioPin, level ? "up" : "down");
    pGpioNode->mMessaging.publish(channel, &button_press);
}