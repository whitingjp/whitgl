#/usr/bin/bash
set -e
sudo apt-get install ninja-build libglfw-dev libglew-dev libpng12-dev xorg-dev libglu1-mesa-dev wget libpng12-dev cmake
rm -rf input
mkdir input
pushd input
wget https://github.com/martine/ninja/raw/master/misc/ninja_syntax.py
read -p "Download fmodstudioapi10509linux from fmod.org, place in input folder, then press [Enter]"
tar -xzf fmodstudioapi10509linux.tar.gz
ln -s fmodstudioapi10509linux fmod
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
