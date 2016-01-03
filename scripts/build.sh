#!/bin/bash
unamestr=`uname`
if [[ "$unamestr" == 'Darwin' ]]; then
	ninja='input/ninja/ninja'
elif [[ "$unamestr" == 'MINGW32_NT-6.2' ]]; then
	ninja='input/ninja.exe'
else
	ninja='ninja'
fi
set -e
pushd whitgl
python build.py
$ninja
popd
python build.py
$ninja
