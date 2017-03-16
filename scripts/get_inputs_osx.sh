#/usr/bin/bash
export MACOSX_DEPLOYMENT_TARGET=10.6
set -e
mkdir input
pushd input
git clone git://github.com/martine/ninja.git
pushd ninja
git checkout release
./configure.py --bootstrap
popd
curl -O https://raw.githubusercontent.com/martine/ninja/master/misc/ninja_syntax.py
curl -O http://www.ambiera.at/downloads/irrKlang-64bit-1.5.0.zip
unzip irrKlang-64bit-1.5.0.zip
ln -s irrKlang-64bit-1.5.0 irrklang
git clone --depth=1 https://github.com/glfw/glfw.git
mkdir glfw/build
pushd glfw/build
cmake .. -DGLFW_BUILD_EXAMPLES=off -DGLFW_BUILD_TESTS=off
make
popd
git clone --depth=1 https://github.com/nigels-com/glew.git
pushd glew
make extensions
make
popd
git clone --depth=1 git@github.com:glennrp/libpng.git
pushd libpng
make -f scripts/makefile.darwin
popd
git clone --depth=1 git@github.com:madler/zlib
pushd zlib
./configure --static
make
popd
git clone --depth=1 https://github.com/MersenneTwister-Lab/TinyMT
pushd TinyMT/tinymt
make
popd
