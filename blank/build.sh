#!/bin/bash
killall game
args=$(<args.txt)
set -e
./whitgl/scripts/build.sh
unamestr=`uname`
if [[ "$unamestr" == 'Darwin' ]]; then
        game_dir='build/out/Game.app/Contents/MacOS'
else
        game_dir='build/out'
fi
(cd $game_dir; ./game hotreload $args "$@" &)
