#!/bin/bash

sudo add-apt-repository ppa:glasen/freetype2
sudo apt-get update
sudo apt-get -y install libglfw3-dev freetype2-demos libglew-dev
sudo apt-get -y install libboost-filesystem-dev -

cd build/
make server -j4
make linux -j4
