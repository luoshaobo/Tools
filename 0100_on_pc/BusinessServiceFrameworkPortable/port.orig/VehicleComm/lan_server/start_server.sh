#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: start_server.sh [ihu | vgm] [ip] [udp | tcp]"
    exit 1
fi

if [ "$1" != "ihu" ] && [ "$1" != "vgm" ]; then
    echo "Wrong server-type '$1'. Should be 'ihu' or 'vgm'"
    exit 1
fi

if [ "$3" != "udp" ] && [ "$3" != "tcp" ]; then
    echo "Wrong server-type '$3'. Should be 'udp' or 'tcp'"
    exit 1
fi

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/src/gcc4.8/lib64/

CONFIG_FILE="$1_server_$3.json"
echo "Config-file: $CONFIG_FILE"

BASE=`grep -ris "unicast" vgm_server_udp.json`
NEWIP="    \"unicast\" : \"$2\","

if [ -z "$2" ]; then
	echo "No argument supplied using default ip-setting"
else
	sed -i "s+$BASE+$NEWIP+g" $CONFIG_FILE
fi

env VSOMEIP_CONFIGURATION=$CONFIG_FILE VSOMEIP_APPLICATION_NAME=VGMServer ./build/build_lan_server/$1_server
