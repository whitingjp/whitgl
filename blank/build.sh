#!/bin/bash
killall game
args=$(<args.txt)
set -e
./whitgl/scripts/build.sh
(cd build/out/Game.app/Contents/MacOS; ./game hotreload $args "$@" &)
