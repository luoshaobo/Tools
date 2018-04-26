#!/bin/bash

HV_FILE="$1"
NK_BIN_WIN_PATH="$2"

WORK_DIR="~hv.tmp"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <hv_file_name> <nk_bin_file_win_path>"
    echo ""
}

if [ $# -lt 2 ]; then
    usage
    exit 0
fi

if [ ! -f "$NK_BIN_WIN_PATH" ]; then
    echo "*** ERROR: the file \"$NK_BIN_WIN_PATH\" does not exist!" >&2
    exit -1
fi

if [ ! -f "$WORK_DIR/boot.reg" ]; then
    echo "*** ERROR: the file \"$WORK_DIR/boot.reg\" does not exist!" >&2
    exit -1
fi

echo "cd \"$WORK_DIR\""
cd "$WORK_DIR"

echo "### cp -pf \"boot.reg\" \"boot.rgu\""
cp -pf "boot.reg" "boot.rgu"

echo "### export _FLATRELEASEDIR=."
export _FLATRELEASEDIR=.
echo "### rgucomp.exe -b"
rgucomp.exe -b

echo "### cp -pf \"boot.hv\" \"../$HV_FILE\""
cp -pf "boot.hv" "../$HV_FILE"

echo "### cd .."
cd ..

echo "### binmod -i \"$NK_BIN_WIN_PATH\" -r \"$HV_FILE\""
binmod -i "$NK_BIN_WIN_PATH" -r "$HV_FILE"

echo "### rm -f \"$HV_FILE\""
rm -f "$HV_FILE"
