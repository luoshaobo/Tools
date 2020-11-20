#!/bin/bash

FILE_PATHNAME="$1"
FILE_BASENAME="`basename $FILE_PATHNAME`"
FILE_7Z_BASENAME="$FILE_BASENAME.7z"
FILE_7Z_PATHNAME="/tmp/$FILE_7Z_BASENAME"

TARGET_USER_AT_HOST="luoshaobo@119.3.91.17"
TARGET_TMP_DIR="~/tmp"

function usage()
{
    echo "Usage:"
    echo "    `basename $0` <file>"
}

function main()
{
    if [ $# -le 0 ]; then
        usage
        exit 1
    fi

    ls -alh "$FILE_PATHNAME"
    echo ""
    
    7z a "$FILE_7Z_PATHNAME" "$FILE_PATHNAME"
    ls -alh "$FILE_7Z_PATHNAME"

    ssh $TARGET_USER_AT_HOST "rm -f $TARGET_TMP_DIR/$FILE_BASENAME"

    while :; do
        time rsync -p -P --rsh='ssh -2' $FILE_7Z_PATHNAME $TARGET_USER_AT_HOST:$TARGET_TMP_DIR
        #time rsync -p -P --rsh='ssh -2 -C' $FILE_7Z_PATHNAME $TARGET_USER_AT_HOST:$TARGET_TMP_DIR
        #time rsync -z -p -P --rsh='ssh -2' $FILE_7Z_PATHNAME $TARGET_USER_AT_HOST:$TARGET_TMP_DIR
        ERRNO=$?
        echo "\$ERRNO=$ERRNO"
        if [ $ERRNO -eq 0 ]; then
            ssh $TARGET_USER_AT_HOST "(cd $TARGET_TMP_DIR; 7z x $FILE_7Z_BASENAME)"
            break
        elif [ $ERRNO -eq 20 ]; then      # cancelled by Ctrl+C
            break
        fi
    done
}

time main $*
