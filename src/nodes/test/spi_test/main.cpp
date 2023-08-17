/*
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "spdlog/spdlog.h"


int spiTxRx(int fd)
{

    unsigned char rxDat;

    struct spi_ioc_transfer spi;

    memset (&spi, 0, sizeof (spi));

    int ledDataLen = 3*3;

    unsigned char txData[] = {1,2,3, 4,5,6, 7,8,9};

    spi.tx_buf        = (unsigned long)&txData;
    spi.rx_buf        = (unsigned long)&rxDat;
    spi.len           = ledDataLen;
    spi.speed_hz      = 20'000'000;

    int rc = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
    spdlog::info("SPI_IOC_MESSAGE: {}", rc);

    return rxDat;
}

int main ()
{

    spdlog::info("hello!");

    int fd = open("/dev/spidev0.0", O_RDWR);

    spdlog::info("fd: {}", fd);

    while (true)
    {

        unsigned char result = spiTxRx(fd);
        spdlog::info("tx result: {}", result);
        //usleep(10);

    }
}

 */


/*
 * spi-pigpio-speed.c
 * 2016-11-23
 * Public Domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <pigpio.h>


int main(int argc, char *argv[])
{

    int loops = 50;
    int bytes = 400*3*24; //400*3;
    int speed = 10'000'000;
    char buf[bytes];

    for(int i = 0; i < bytes; i++)
    {
        buf[i] = i;
    }

    if (gpioInitialise() < 0) return 1;

    int h = spiOpen(0, speed, 0);

    if (h < 0) return 2;


    double start = time_time();

    for (int i=0; i<loops; i++)
    {
        spiWrite(h, buf, bytes);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    double diff = time_time() - start;

    printf("sps=%.1f: %d bytes @ %d bps (loops=%d time=%.5f, max time = 0.02 @ 50hz)\n",
           (double)loops / diff, bytes, speed, loops, diff);


    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    spiClose(h);

    gpioTerminate();

    return 0;
}
