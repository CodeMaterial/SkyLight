# System Wide Stuff

### Dependencies
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git cmake build-essential libglib2.0-dev python3-dev valgrind
```

### Perf nonsense
```
wget http://ftp.uk.debian.org/debian/pool/main/l/linux/linux-perf_6.1.20-2~bpo11+1_arm64.deb
sudo dpkg -i linux-perf_6.1.20-2~bpo11+1_arm64.deb 
sudo apt-get -f install
sudo dpkg -i linux-perf_6.1.20-2~bpo11+1_arm64.deb 
```

### toml
```
git clone https://github.com/cktan/tomlcpp
cd tomlcpp
make
sudo make install
```

### LCM
```
git clone https://github.com/lcm-proj/lcm
cd lcm
mkdir build
cd build
cmake ..
make
sudo make install
```

### Spdlog install
```
sudo apt-get install libspdlog-dev
```

# Speech Node

### PocketSphinx requirements
```
sudo apt-get install gcc automake autoconf libtool bison swig python-dev libasound2-dev
```

### Sphinxbase install
```
cd ~/git
git clone https://github.com/cmusphinx/sphinxbase
cd /sphinxbase
./autogen.sh
./configure
make clean all
make check
sudo make install
```

### Pocketsphinx install
```
git clone https://github.com/cmusphinx/pocketsphinx
cd ~/git/pocketsphinx
./autogen.sh 
./configure 
make clean all 
make check 
sudo make install
```

### Set Pocketsphinx library paths
```
export LD_LIBRARY_PATH=/usr/local/lib
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

# Sound Node

### Enable audio daq hat
```
sudo echo 'dtoverlay=hifiberry-dac' >> /boot/config.txt
# comment #dtparam=audio=on maybe?
```

### Enable soft vol
write the following to `/etc/asound.conf`
```
pcm.softvol { 
    type softvol 
    slave { 
        pcm "default"
    }
    control { 
        name "SoftMaster" 
        card 0
    }
}
```


sudo sh -c 'echo 1 > /proc/sys/kernel/perf_event_paranoid'