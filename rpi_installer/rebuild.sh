#!/usr/bin/sh

cd build

sudo mkdir -p CMakeFiles
sudo cmake .. -DOPT_BUILD_RTL_SDR_SOURCE=ON -DOPT_BUILD_SDRPLAY_SOURCE=ON

echo "Re-Building"
sudo make -j4 -VERBOSE=1

echo "Installing"
sudo make install

echo "Done!"

### My custom SDR++ Rebuilder
