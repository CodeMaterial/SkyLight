#include "gpio_node.h"

int main(int argc, char **argv) {

    skylight::GpioNode gpioNode;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}