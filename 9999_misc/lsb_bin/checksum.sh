#!/bin/bash

TARGET_PATH="$1"

function usage()
{
    echo "Usage: "
    echo "  $0 <path_to_be_processed>"
    echo ""
}

function process_current_dir()
{
    while read LINE; do
        echo $LINE
        if [ -f "$LINE" ]; then
            echo -e -n "    Size (Bytes): "
            du -b "$LINE" | awk '{ print $1 }'
            echo -e -n "    MD5 Checksum: "
            md5sum -b "$LINE" | awk '{ print $1 }'
        fi
    done < <(find .)
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

if [ "$1" == "-H" -o "$1" == "--help" ]; then
    usage
    exit 0
fi

if [ ! -d "$TARGET_PATH" ]; then
    echo "*** ERROR: \"$TARGET_PATH\" is not a directory!" >&2
    exit -1
fi

TARGET_PATH=$(cd "$TARGET_PATH" && pwd)

(cd "$TARGET_PATH" && process_current_dir)
