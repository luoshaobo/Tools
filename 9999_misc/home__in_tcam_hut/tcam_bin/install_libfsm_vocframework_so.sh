#!/bin/bash

TARGET_MOUNT_DIR="$HOME/opt1"
NV=${TCAM_BUILD_ROOT:="$DEFAULT_TCAM_BUILD_ROOT"}

DST_FILE_PATH_IN_PC="$TCAM_BUILD_ROOT/release/fs/devel/usr/lib/libfsm_vocframework.so"
TMP_DIR="/tmp/tcam"
TMP_DST_FILE="$TMP_DIR/`basename $DST_FILE_PATH_IN_PC`"
DST_DIR_IN_TARGET="$TARGET_MOUNT_DIR/usr/lib"

function main
{
    if [ ! -f "$TARGET_MOUNT_DIR/SmeDbgLog.txt" -a ! -h "$TARGET_MOUNT_DIR/SmeDbgLog.txt" ]; then
        echo "*** ERROR: can't visit the target file system!" >&2
        return 1
    fi
    
    if [ ! -f "$DST_FILE_PATH_IN_PC" ]; then
        echo "*** ERROR: "$DST_FILE_PATH_IN_PC" does not exist!" >&2
        return 1
    fi

    mkdir -p "$TMP_DIR"
    echo "cp -pf \"$DST_FILE_PATH_IN_PC\" \"$TMP_DIR/\""
    cp -pf "$DST_FILE_PATH_IN_PC" "$TMP_DIR/"

    echo "arm-cas-linux-gnueabi-strip \"$TMP_DST_FILE\""
    arm-cas-linux-gnueabi-strip "$TMP_DST_FILE"
    
    rm "$DST_DIR_IN_TARGET/`basename $DST_FILE_PATH_IN_PC`"
    echo "cp \"$TMP_DST_FILE\" \"$DST_DIR_IN_TARGET/\""
    cp "$TMP_DST_FILE" "$DST_DIR_IN_TARGET/"
}

. ~/bin/set_tcam_gcc_env.sh
main
