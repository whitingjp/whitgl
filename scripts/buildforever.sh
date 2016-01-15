#!/bin/bash
while sleep 1; do
find build.py build.sh src data scripts whitgl/src whitgl/inc | entr -d -c ./build.sh "$@"
done
