#!/bin/bash

# ensure that if anything fails then return an error code
set -ev

sudo apt-get -y install git cmake build-essential libglib2.0-dev python3-dev valgrind

sudo rm -rf dependencies
mkdir dependencies && cd dependencies

git clone https://github.com/cktan/tomlcpp
cd tomlcpp
make toml_json
sudo make install toml_json
cd ..


git clone https://github.com/lcm-proj/lcm
cd lcm
mkdir build && cd build
cmake ..
make
sudo make install
cd ..


#libspdlog-dev is only available for debian
sudo apt-get -y install libspdlog-dev ||
{
  git clone https://github.com/gabime/spdlog
  cd spdlog
  mkdir build && cd build
  cmake ..
  make
  sudo make install
  cd ..
}

cd ..

sudo rm -rf dependencies