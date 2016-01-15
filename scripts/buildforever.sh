#!/bin/bash
while sleep 1; do
find build.py build.sh args.txt src data scripts whitgl/src whitgl/inc | entr -d -c ./build.sh "$@"
done
