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

echo "### rm -fr \"$WORK_DIR\""
rm -fr "$WORK_DIR"
echo "### mkdir -p \"$WORK_DIR\""
mkdir -p "$WORK_DIR"

echo "### binmod -i \"$NK_BIN_WIN_PATH\" -e \"$HV_FILE\""
binmod -i "$NK_BIN_WIN_PATH" -e "$HV_FILE"
echo "### mv \"$HV_FILE\" \"$WORK_DIR/orig_$HV_FILE\""
mv "$HV_FILE" "$WORK_DIR/orig_$HV_FILE"

echo "cd \"$WORK_DIR\""
cd "$WORK_DIR"

echo "### rgucomp -o \"orig_$HV_FILE\" -nologo > temp.rgu"
rgucomp -o "orig_$HV_FILE" -nologo > temp.rgu

echo "### rgu_ascii_to_unicode.sh temp.rgu boot.rgu"
rgu_ascii_to_unicode.sh temp.rgu boot.rgu

echo "### mv boot.rgu boot.reg"
mv boot.rgu boot.reg

echo "**********************************************************************************"
echo "**!!! Now you can edit $WORK_DIR/boot.reg in vs 2008, then execute:"
echo "**        modify_hv_in_nk_bin_end.sh \"$HV_FILE\" \"$NK_BIN_WIN_PATH\""
echo "**    to update your *.bin."
echo "**********************************************************************************"
