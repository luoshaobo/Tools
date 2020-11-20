#!/bin/bash

FILEPATH_IN_DEV_FS="$1"     # e.g. /usr/bin/voc
FILEDIR_IN_TARGET="$2"      # e.g. /usr/bin

TARGET_MOUNT_DIR="$HOME/opt1"
NV=${TCAM_BUILD_ROOT:="$DEFAULT_TCAM_BUILD_ROOT"}

DST_FILE_PATH_IN_PC="$TCAM_BUILD_ROOT/release/fs/devel$FILEPATH_IN_DEV_FS"
DST_FILE_BASENAME=`basename $DST_FILE_PATH_IN_PC`
TMP_DIR="/tmp/tcam"
TMP_DST_FILE="$TMP_DIR/$DST_FILE_BASENAME"
DST_DIR_IN_TARGET="$FILEDIR_IN_TARGET"
DST_DIR_IN_PC="$TARGET_MOUNT_DIR$DST_DIR_IN_TARGET"
DST_DATA_TMP_IN_TARGET="/data/tmp"
DST_DATA_TMP_IN_PC="$TARGET_MOUNT_DIR$DST_DATA_TMP_IN_TARGET"

REMOTE_SHELL="dbclient -p 50022 root@127.0.0.1"
export DROPBEAR_PASSWORD=1 # for $REMOTE_SHELL

if [ $# -lt 2 ]; then
    echo "*** ERROR: 2 arguments needed" >&2
    exit 1
fi

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

    rm "$DST_DIR_IN_PC/$DST_FILE_BASENAME"

    echo "cat \"$TMP_DST_FILE\" | bzip2 -9 > \"$TMP_DST_FILE.bz2\""
    cat "$TMP_DST_FILE" | bzip2 -9 > "$TMP_DST_FILE.bz2"

    mkdir -p "$DST_DATA_TMP_IN_PC"
    echo "cp \"$TMP_DST_FILE.bz2\" \"$DST_DATA_TMP_IN_PC\""
    cp "$TMP_DST_FILE.bz2" "$DST_DATA_TMP_IN_PC/"
    
    mkdir -p "$DST_DIR_IN_PC"

    echo "$REMOTE_SHELL \"(cd $DST_DATA_TMP_IN_TARGET && bunzip2 -f $DST_FILE_BASENAME.bz2 && chmod a+x $DST_FILE_BASENAME && mv $DST_FILE_BASENAME $DST_DIR_IN_TARGET)\""
    $REMOTE_SHELL "(cd $DST_DATA_TMP_IN_TARGET && bunzip2 -f $DST_FILE_BASENAME.bz2 && chmod a+x $DST_FILE_BASENAME && mv $DST_FILE_BASENAME $DST_DIR_IN_TARGET)"

    $REMOTE_SHELL "sync"

    ls -l "$TMP_DST_FILE"
    ls -l "$DST_DIR_IN_PC/$DST_FILE_BASENAME"
}

. ~/bin/set_tcam_gcc_env.sh
time main
