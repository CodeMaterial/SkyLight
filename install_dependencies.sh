#!/bin/bash

sudo apt-get -y install git cmake build-essential libglib2.0-dev python3-dev valgrind libspdlog-dev

sudo rm -rf dependencies
mkdir dependencies && cd dependencies || exit

git clone https://github.com/cktan/tomlcpp
cd tomlcpp || exit
make tomlcpp
sudo make install
cd ..


git clone https://github.com/lcm-proj/lcm
cd lcm || exit
mkdir build && cd build || exit
cmake ..
make
sudo make install
cd ..