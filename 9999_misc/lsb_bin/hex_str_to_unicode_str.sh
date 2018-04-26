#!/bin/bash

function usage()
{
    echo "Usage:"
    echo "  `basename $0` [-b|--bom] <hex_str>"
    echo "Note:"
    echo "1) The format of <hex_str> should be as below:"
    echo "     0028 006E 0061 006D 0065 0020 0030 0030 0034 0029"
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit 0
fi

if [  "$1" == "-b" -o "$1" == "--bom" ]; then
    echo -n -e "\xFF\xFE"
    
    if [ $# -lt 2 ]; then
        usage
        exit 0
    fi
    CONTENT="$2"
else
    CONTENT="$1"
fi

for UCH in $CONTENT; do
    if [ ${#UCH} -ne 4 ]; then
        echo "*** ERROR: \"$UCH\" is not an unicode char!" >&2
        exit 1
    fi
    echo -n -e "\x${UCH:2:2}\x${UCH:0:2}"
done
