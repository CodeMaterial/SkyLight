#!/bin/bash

# ensure that if anything fails then return an error code
set -ev

sudo apt-get -y install git cmake build-essential libglib2.0-dev python3-dev valgrind

sudo rm -rf dependencies
mkdir dependencies && cd dependencies

git clone https://github.com/cktan/tomlcpp
cd tomlcpp || exit
sudo make install
cd ..


git clone https://github.com/lcm-proj/lcm
cd lcm || exit
mkdir build && cd build
cmake ..
make
sudo make install
cd ..


#we could use sudo apt-get install libspdlog-dev for debian, however it's more portable to build from source
git clone https://github.com/gabime/spdlog
cd spdlog || exit
mkdir build && cd build
cmake ..
make
sudo make install
cd ..