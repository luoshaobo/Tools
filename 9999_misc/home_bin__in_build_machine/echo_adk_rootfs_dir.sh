#!/bin/bash

VERSION="$1"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <VERSION>"
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit
fi

ROOTFS_DIR=`(cd $HOME/SyDK_${VERSION}_*/NAVI_NAC_CN/imx6x-std-adk/armv7a-mv-linux && pwd ) 2>/dev/null`
if [ $? -ne 0 ]; then
    ROOTFS_DIR=`(cd $HOME/SyDK_${VERSION}_*/NAVINFO_NAC_CN/imx6x-std-adk/armv7a-mv-linux && pwd ) 2>/dev/null`
fi

if [ $? -ne 0 ]; then
    exit
fi

echo "set sysroot $ROOTFS_DIR"
echo "set solib-search-path $ROOTFS_DIR/lib"
echo "set solib-search-path $ROOTFS_DIR/usr/lib"
