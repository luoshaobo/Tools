#!/bin/bash

function usage()
{
    echo "Usage:"
    echo "  `basename $0` [-b|--bom] <hex_str>"
    echo "Note:"
    echo "1) The format of <hex_str> should be as below:"
    echo "     68 74 74 70 3A 2F 2F 31 39 32 2E 31 36 38 2E 31 2E 32 35 3A 38 30 2F 54 65 6C"
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit 0
fi

if [  "$1" == "-b" -o "$1" == "--bom" ]; then
    echo -n -e "\xEF\xBB\xBF"
    
    if [ $# -lt 2 ]; then
        usage
        exit 0
    fi
    CONTENT="$2"
else
    CONTENT="$1"
fi

for CH in $CONTENT; do
    if [ ${#CH} -ne 2 ]; then
        echo "*** ERROR: \"$CH\" is not an byte!" >&2
        exit 1
    fi
    echo -n -e "\x${CH}"
done
