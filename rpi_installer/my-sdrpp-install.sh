#!/usr/bin/sh
set -e

[ $(id -u) = 0 ] && echo "Please do not run this script as root" && exit 100

echo "Installing dependencies"
sudo apt update
sudo apt install -y build-essential cmake git libfftw3-dev libglfw3-dev libglew-dev libvolk2-dev libzstd-dev libsoapysdr-dev libairspyhf-dev libairspy-dev \
            libiio-dev libad9361-dev librtaudio-dev libhackrf-dev librtlsdr-dev libbladerf-dev liblimesuite-dev p7zip-full wget
            
git clone https://github.com/Hagtronics/customSDRpp
cd SDRPlusPlus            

echo "Preparing build"
sudo mkdir -p build
cd build

sudo mkdir -p CMakeFiles
sudo cmake .. -DOPT_BUILD_RTL_SDR_SOURCE=ON -DOPT_BUILD_SDRPLAY_SOURCE=ON

echo "Building"
sudo make -j4

echo "Installing"
sudo make install

echo "Done!"

### My custom SDR++ Installer
