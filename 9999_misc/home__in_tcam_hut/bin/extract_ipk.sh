#!/bin/bash

ROOT_IPK_PATH="$1"
WORK_DIR="$2"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <IPK_PATH> <WORK_DIR>"
    echo ""
}

if [ $# -lt 2 ]; then
    usage
    exit
fi

if [ ! -f "$ROOT_IPK_PATH" ]; then
    echo "*** Error: not a ipk file: $ROOT_IPK_PATH" >&2
    exit 1
fi

if [ ! -d "$WORK_DIR" ]; then
    echo "*** Error: wrong WORK_DIR: $WORK_DIR" >&2
    exit 1
fi

ROOT_IPK_DIR=`dirname "$ROOT_IPK_PATH"`
ROOT_IPK_FILE_WITH_EXT=`basename "$ROOT_IPK_PATH"`
ROOT_IPK_FILE_WITHOUT_EXT=`basename "$ROOT_IPK_PATH" .ipk`

mkdir -p "$WORK_DIR/$ROOT_IPK_FILE_WITHOUT_EXT"
cp "$ROOT_IPK_PATH" "$WORK_DIR/$ROOT_IPK_FILE_WITHOUT_EXT"
(
    cd "$WORK_DIR/$ROOT_IPK_FILE_WITHOUT_EXT"
    ar x "$ROOT_IPK_FILE_WITH_EXT"
    tar zxf control.tar.gz
    tar zxf data.tar.gz
    rm -f control.tar.gz data.tar.gz
    rm -f "$ROOT_IPK_FILE_WITH_EXT"
)

SUB_IPKS=`find "$WORK_DIR/$ROOT_IPK_FILE_WITHOUT_EXT" -name "*.ipk"`
for SUB_IPK in $SUB_IPKS; do
    if [ -n "$SUB_IPK" ]; then
        SUB_IPK_DIR=`dirname "$SUB_IPK"`
        SUB_IPK_FILE_WITH_EXT=`basename "$SUB_IPK"`
        SUB_IPK_FILE_WITHOUT_EXT=`basename "$SUB_IPK" .ipk`
        (extract_ipk.sh "$SUB_IPK" "$SUB_IPK_DIR")
    fi
done
