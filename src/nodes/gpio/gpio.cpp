#include "gpio.h"
#include "spdlog/spdlog.h"


skylight::GPIO::GPIO(gpioAlertFuncEx_t buttonCallback, void *buttonCallbackContext) {

    spdlog::info("skylight gpio initialising");

    if (gpioInitialise() < 0) {
        throw std::runtime_error("skylight gpio system failed to initialise pigpio");
    }

    skylight::Config pConfig = skylight::GetConfig("skylight_gpio.toml");

    int speed = skylight::getConfigInt(pConfig, "spi_speed");
    int spiDevice = skylight::getConfigInt(pConfig, "spi_device");

    mpSPI = std::make_shared<SPI>(spiDevice, speed);

    std::vector<long int> pGPIO_ports = *pConfig->getArray("gpio_ports")->getIntVector();

    if (pGPIO_ports.empty()) {
        spdlog::warn("gpio system failed to load pin numbers from config, assuming no buttons registered");
    }

    int debounce = skylight::getConfigInt(pConfig, "gpio_debounce");

    for (long int GPIO_port: pGPIO_ports) {
        spdlog::info("button system setting up gpio port {} with debounce of {}", GPIO_port, debounce);
        gpioSetMode(GPIO_port, PI_INPUT);
        gpioSetPullUpDown(GPIO_port, PI_PUD_UP);
        gpioGlitchFilter(GPIO_port, debounce);
        gpioSetAlertFuncEx(GPIO_port, buttonCallback, buttonCallbackContext);
    }

    spdlog::info("skylight gpio initialized");
}

skylight::GPIO::~GPIO() {
    gpioTerminate();
}


void skylight::GPIO::Update() {
    if (!mpSPI->SendUpdateCommand()) {
        spdlog::error("skylight gpio failed to send correct number of bytes during the update command");
    }
}

void skylight::GPIO::ReceiveBuffer(const skylight_message::pixel_buffer *pMsg) {
    if (!mpSPI->SendBufferToHardware(pMsg)) {
        spdlog::error("skylight gpio failed to send correct number of bytes during the buffer update command");
    }
}
