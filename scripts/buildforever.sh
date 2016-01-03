#!/bin/bash
while sleep 1; do
find build.py build.sh src data whitgl/src | entr -d -c ./build.sh
done
