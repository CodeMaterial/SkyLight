# SkyLight

Project Skylight is a collection of nodes that you can easily configure to control electronic fursuits

# Installation

```shell
./install_dependencies.sh
./build_and_install.sh
```

# Running nodes

`sudo skylight_button` - runs the node in the terminal

`sudo systemctl start skylight_button` - runs the node in the background

`sudo systemctl enable [node name]` - runs the node on startup

# Configuring nodes

`sudo nano /usr/local/etc/skylight_button.toml`
```
#config for the button system!

GPIO_ports = [17,27]
debounce = 10 #microseconds
```
Nodes will look in the current working directory for config files, then the parent, then `/usr/local/etc`

# Configuring node communication
`sudo nano /usr/local/etc/topic_config.toml`

```
[routing]

"button_17_down" = "user/voice_start"
"button_17_up" = "user/voice_stop"
"button_27_down" = ["user/confirm", "user/trigger"]
```

# Nodes!

## skylight_button
Converts button inputs to `user_command` messages
By default configured for GPIO ports 17 & 27 with a debounce of 10 microseconds.

## skylight_cli
To be used directly from the command line with `skylight_cli`.
Broadcasts `user_command` messages on a particular channel defined by the argument `-c`

# Message types!

## user_command
A user command
```
int64_t    timestamp;
string     command;
```

## pixel_buffer
A pixel buffer
```
int64_t     timestamp;
byte        buffer[10'000][3];
```

# Rebuilding and re-installing a single node
note - If you are building the modules individually and haven't run `build_and_install.sh`, please copy `src/config/topic_config.toml` to `/usr/local/etc` manually
```shell
cd src/nodes/button
mkdir build
cd build
cmake ..
make
sudo make install
```
