#!/bin/bash

# debian folder created by dh_make --createorig --single -c mit -e acornejo@gmail.com

APPNAME=kilogui
VERSION=1.0

tar czvf ../${APPNAME}_${VERSION}.orig.tar.gz . --exclude .git --exclude debian --transform="s,^\.,${APPNAME}-${VERSION},"
dpkg-buildpackage
