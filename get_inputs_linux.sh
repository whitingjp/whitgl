#/usr/bin/bash
sudo apt-get install ninja-build libglfw-dev libglew-dev libpng12-dev xorg-dev libglu1-mesa-dev
rm -rf input
mkdir input
pushd input
wget https://github.com/martine/ninja/raw/master/misc/ninja_syntax.py
wget http://www.fmod.org/download/fmodstudio/api/Linux/fmodstudioapi10509linux.tar.gz
tar -xzf fmodstudioapi10509linux.tar.gz
ln -s fmodstudioapi10509linux fmod
wget https://github.com/glfw/glfw/archive/3.0.4.tar.gz
tar -xzf 3.0.4.tar.gz
ln -s glfw-3.0.4 glfw
mkdir glfw/build
pushd glfw/build
cmake ..
make
popd
