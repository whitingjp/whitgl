#/usr/bin/bash
sudo apt-get install ninja-build libglfw-dev libglew-dev
rm -rf input
mkdir input
pushd input
wget https://github.com/martine/ninja/raw/master/misc/ninja_syntax.py
wget http://www.fmod.org/download/fmodex/api/Linux/fmodapi44434linux.tar.gz
tar -xzf fmodapi44434linux.tar.gz
ln -s fmodapi44434linux fmod
popd
