#!/bin/bash

export PATH=/cygdrive/d/sw/01_installed/NAD_Flashing_Tool:$PATH

SW_PACKAGES_DIR="$1"

if [ $# -lt 1 ]; then
    CURRENT_DIR_UNIX=`pwd`
    CURRENT_DIR_WIN=`cygpath -a -w "$CURRENT_DIR_UNIX"`
    SW_PACKAGES_DIR="$CURRENT_DIR_WIN"
fi

if [ ! -f "$SW_PACKAGES_DIR/ubi.img" ]; then
    echo "*** ERROR: $SW_PACKAGES_DIR is not the software package directory!" >&2
    exit 1
fi

echo "NFT.exe -f=full -sw=$SW_PACKAGES_DIR"
NFT.exe -f=full -sw=$SW_PACKAGES_DIR
