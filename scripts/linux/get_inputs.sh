#/usr/bin/bash
set -e
sudo apt-get install ninja-build libglfw3-dev libglew-dev xorg-dev libglu1-mesa-dev wget libpng-dev cmake
rm -rf input
mkdir input
pushd input
wget https://github.com/martine/ninja/raw/master/misc/ninja_syntax.py
wget http://www.ambiera.at/downloads/irrKlang-64bit-1.5.0.zip
unzip irrKlang-64bit-1.5.0.zip
ln -s irrKlang-64bit-1.5.0 irrklang
git clone --depth=1 https://github.com/glfw/glfw.git
mkdir glfw/build
pushd glfw/build
cmake ..
make
popd
git clone --depth=1 https://github.com/MersenneTwister-Lab/TinyMT
pushd TinyMT/tinymt
make
popd
