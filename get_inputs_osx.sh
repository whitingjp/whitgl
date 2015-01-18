#/usr/bin/bash
set -e
mkdir input
pushd input
git clone git://github.com/martine/ninja.git
pushd ninja
git checkout release
./configure.py --bootstrap
popd
curl -O https://raw.githubusercontent.com/martine/ninja/master/misc/ninja_syntax.py
curl -O http://www.fmod.org/download/fmodstudio/api/Mac/fmodstudioapi10510mac-installer.dmg 
hdiutil mount fmodstudioapi10510mac-installer.dmg
cp -r /Volumes/FMOD\ Programmers\ API\ Mac/FMOD\ Programmers\ API/api/lowlevel/ fmod
hdiutil unmount /Volumes/FMOD\ Programmers\ API\ Mac/
git clone https://github.com/glfw/glfw.git
mkdir glfw/build
pushd glfw/build
cmake ..
make
popd
git clone https://github.com/nigels-com/glew.git
pushd glew
make extensions
make
