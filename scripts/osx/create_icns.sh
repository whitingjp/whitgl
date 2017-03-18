#/usr/bin/bash
set -e
if [ $# -ne 2 ]
  then
    echo "Usage:"
    echo "  create_icns.sh source_image.png working_dir"
    exit 1
fi

SOURCE="$1"
WORKING_DIR="$2"
ICONSET=${WORKING_DIR}/Icon.iconset

mkdir -p ${ICONSET}
sips -z 16 16     ${SOURCE} --out ${ICONSET}/icon_16x16.png
sips -z 32 32     ${SOURCE} --out ${ICONSET}/icon_16x16@2x.png
sips -z 32 32     ${SOURCE} --out ${ICONSET}/icon_32x32.png
sips -z 64 64     ${SOURCE} --out ${ICONSET}/icon_32x32@2x.png
sips -z 128 128   ${SOURCE} --out ${ICONSET}/icon_128x128.png
sips -z 256 256   ${SOURCE} --out ${ICONSET}/icon_128x128@2x.png
sips -z 256 256   ${SOURCE} --out ${ICONSET}/icon_256x256.png
sips -z 512 512   ${SOURCE} --out ${ICONSET}/icon_256x256@2x.png
sips -z 512 512   ${SOURCE} --out ${ICONSET}/icon_512x512.png
cp ${SOURCE} ${ICONSET}/icon_512x512@2x.png
iconutil -c icns ${ICONSET}
