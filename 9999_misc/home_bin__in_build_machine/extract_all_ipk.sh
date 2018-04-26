#!/bin/bash

IPKS_DIR="$1"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <IPKS_DIR>"
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit
fi

#IPKS=`find /PROJ/oip/GWM_MY18/MY18_SyDK/SyDK_11.17.13.01_20170328/Mirror/solutions/com.continental/MV_SYSINT/prebuilt/arm-mv-linux -name "*.ipk"`
IPKS=`find "$IPKS_DIR" -name "*.ipk"`

for IPK in $IPKS; do
    FILENAME_WHITOUT_EXT=`basename "$IPK" .ipk`
    mkdir -p "$FILENAME_WHITOUT_EXT"
    extract_ipk.sh "$IPK" "$FILENAME_WHITOUT_EXT"
    #exit
done

