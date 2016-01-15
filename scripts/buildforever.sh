#!/bin/bash
while sleep 1; do
find "$@" | entr -d -c ./build.sh
done
