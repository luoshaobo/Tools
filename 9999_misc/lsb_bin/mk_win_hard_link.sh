#!/bin/bash

LINK_NAME="$1"
REAL_PATH="$2"

function usage()
{
    echo "Usage: "
    echo "  $0 <link_name> <real_path>"
    echo ""
}

if [ $# -lt 2 ]; then
    usage
    exit 1
fi

if [ "$1" == "-H" -o "$1" == "--help" ]; then
    usage
    exit 0
fi

if [ ! -e "$REAL_PATH" ]; then
    echo "*** ERROR: \"$REAL_PATH\" is not a file or directory!" >&2
    exit 2
fi

if [ -L "$LINK_NAME" ]; then
    unlink "$LINK_NAME"
fi

# if [ -e "$LINK_NAME" ]; then
    # echo "*** ERROR: \"$LINK_NAME\" exits already!" >&2
    # exit 3
# fi

rm -f $LINK_NAME
echo "cmd /C mklink /H $LINK_NAME $REAL_PATH"
cmd /C mklink /H $LINK_NAME $REAL_PATH
