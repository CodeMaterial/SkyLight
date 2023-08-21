#!/bin/bash

# ensure that if anything fails then return an error code
set -ev

sudo rm -rf build
mkdir build && cd build
cmake ..
make
sudo make install

cd ..