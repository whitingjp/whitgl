set -e
mkdir input
pushd input

DOWNLOAD="cscript.exe ../scripts/get_inputs_windows/downloadfile_windows.vbs"
UNZIP="cscript.exe ../scripts/get_inputs_windows/unzipfile_windows.vbs"
$DOWNLOAD http://github.com/ninja-build/ninja/releases/download/v1.7.2/ninja-win.zip ninja-win.zip
$UNZIP ninja-win.zip
$DOWNLOAD https://raw.githubusercontent.com/martine/ninja/master/misc/ninja_syntax.py ninja_syntax.py
$DOWNLOAD http://www.ambiera.at/downloads/irrKlang-32bit-1.5.0.zip irrKlang-32bit-1.5.0.zip
$UNZIP irrKlang-32bit-1.5.0.zip
mv irrKlang-1.5.0 irrklang
$DOWNLOAD https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN32.zip glfw-3.2.1.bin.WIN32.zip
$UNZIP glfw-3.2.1.bin.WIN32.zip
mv glfw-3.2.1.bin.WIN32 glfw
$DOWNLOAD https://github.com/nigels-com/glew/releases/download/glew-2.0.0/glew-2.0.0-win32.zip glew-2.0.0-win32.zip
$UNZIP glew-2.0.0-win32.zip
mv glew-2.0.0 glew
git clone --depth=1 https://github.com/madler/zlib
pushd zlib
make -f win32/Makefile.gcc
popd
git clone --depth=1 https://github.com/glennrp/libpng
cp ../scripts/get_inputs_windows/libpng_makefile.msys libpng/scripts
pushd libpng
make -f scripts/libpng_makefile.msys
popd
git clone --depth=1 https://github.com/MersenneTwister-Lab/TinyMT
pushd TinyMT/tinymt
make
popd