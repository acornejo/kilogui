#!/bin/bash

MXEPATH=~/devel/mxe/usr/bin
CROSS=i686-pc-mingw32

# Set path
if [[ ":$PATH:" != *":$MXEPATH:"* ]]; then
    export PATH=$MXEPATH:$PATH
fi

if [ -f Makefile ]; then
    make clean
    make distclean
fi

${CROSS}-qmake
make
zip -j kilogui-win32.zip src/release/kilogui.exe
