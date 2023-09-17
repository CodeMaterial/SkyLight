#!/bin/bash

# ensure that if anything fails then return an error code
set -ev

sudo apt-get -y install git cmake build-essential libglib2.0-dev python3-dev valgrind libfmt-dev pigpio

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

#right okay. What we're gonna do here is install an old version of sphinxbase and pocketsphinx.
#this should really be updated to the new version of pocketsphinx. but can I get it to work, can I hell.
#so this janky nastyness is here for now.
#TODO - not this

#install sphinxbase / pocketsphinx dependencies
sudo apt-get install gcc automake autoconf libtool bison swig python-dev libasound2-dev

#install sphinxbase
git clone https://github.com/cmusphinx/sphinxbase
cd sphinxbase
git checkout `git rev-list -n 1 --before="2022-03-01 00:00" master`
./autogen.sh
./configure
make clean all
make
sudo make install
cd ..

#install pocketsphinx
git clone https://github.com/cmusphinx/pocketsphinx
cd pocketsphinx
git checkout `git rev-list -n 1 --before="2022-03-01 00:00" master`
./autogen.sh
./configure
make clean all
make
sudo make install
cd ..

cd ..

sudo rm -rf dependencies