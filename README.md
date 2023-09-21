# SkyLight

Project Skylight is a collection of nodes that you can easily configure to control electronic fursuits

# Installation

```shell
./install_dependencies.sh
./build_and_install.sh
```

# Running nodes

`sudo skylight_gpio` - runs the gpio node in the terminal

`sudo systemctl start skylight_gpio` - runs the node in the background

`sudo systemctl enable skylight_gpio` - runs the node on startup

# Nodes!

---

## skylight_cli

A command line tool used to publish void, string and float commands to any channel

```
Options:
-h,--help                   Print this help message and exit
-c,--channel TEXT REQUIRED  The channel to broadcast on, sends a simple_void message if no other arguments are used
-s,--string TEXT            send a simple_string message
-f,--float FLOAT            send a simple_float message
```

---

# skylight_sound

The sound node creates a sound object for each device listed in `skylight_sound.toml`

Default device names are `internal` and `external`

### Subscribes to

| Topic                         | Type   | Example         |
|-------------------------------|--------|-----------------|
| `/sound/[device name]/speak`  | string | `hello there`   |
| `/sound/[device name]/play`   | string | `~/example.wav` |
| `/sound/[device name]/volume` | float  | `0.4`           |

---

# skylight_speech

### Subscribes to

| Topic           | Message Type |
|-----------------|--------------|
| `/speech/start` | void         |
| `/speech/stop`  | void         |

### Publishes to

| Topic             | Message Type | Example         |
|-------------------|--------------|-----------------|
| `/speech/command` | string       | `start rainbow` |

---

# skylight_gpio

### Subscribes to

| Topic              | Message Type |
|--------------------|--------------|
| `/gpio/update`     | void         |
| `/gpio/led_buffer` | pixel_buffer |

### Publishes to

| Topic                          | Message Type |
|--------------------------------|--------------|
| `/gpio/button_[pin]/[up/down]` | void         |

---

## skylight_effect_driver

### Subscribes to

| Topic                               | Message Type |
|-------------------------------------|--------------|
| `/effect_driver/effects/test/start` | void         |

### Publishes to

| Topic                   | Message Type |
|-------------------------|--------------|
| `/effect_driver/buffer` | void         |
| `/effect_driver/update` | void         |

---

## skylight_effect_driver_and_gpio

This node combines the effect driver and gpio, reducing the latency due to large buffers being sent over the network.
It has the same subs and pubs as their individual nodes.
---

# Configuring nodes

`sudo nano /usr/local/etc/skylight_gpio.toml`

```
#config for the gpio system
spi_speed = 10000000 #10mhz for the spi update
spi_device = 0
gpio_ports = [17, 27]
gpio_debounce = 100 #microseconds
```

All node configuration can be found in `/usr/local/etc/`

# Configuring node communication

`sudo nano /usr/local/etc/topic_config.toml`

```
[routing]

"/effect_driver/led_buffer" = "/gpio/led_buffer"
"/effect_driver/update" = "/gpio/update"

"/gpio/button_27/down" = "/effect_driver/test"
```

# Rebuilding and re-installing a single node

note - If you are building the modules individually and haven't run `build_and_install.sh`, please
copy `src/config/topic_config.toml` to `/usr/local/etc` manually

```shell
cd src/nodes/gpio
mkdir build
cd build
cmake ..
make
sudo make install
```
