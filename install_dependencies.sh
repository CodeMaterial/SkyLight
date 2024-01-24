#!/bin/bash

# ensure that if anything fails then return an error code
set -ev

sudo rm -rf dependencies
mkdir dependencies && cd dependencies

sudo apt-get -y install git cmake build-essential libglib2.0-dev python3-dev valgrind libfmt-dev sox espeak

sudo apt-get -y install pigpio ||
{
  sudo apt-get -y install unzip
  wget https://github.com/joan2937/pigpio/archive/master.zip
  unzip master.zip
  cd pigpio-master
  make
  sudo make install
  cd ..
}

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

# enable the audio-shim, if you install multiple times, this will keep appending
echo 'dtoverlay=hifiberry-dac' | sudo tee --append /boot/config.txt

#setup softvol for audio-shim
echo "pcm.softvol{type softvol slave {pcm \"default\"} control {name \"SoftMaster\" card 0}}" | sudo tee /etc/asound.conf

#And we're done! cleanup time!
cd ..
sudo rm -rf dependencies