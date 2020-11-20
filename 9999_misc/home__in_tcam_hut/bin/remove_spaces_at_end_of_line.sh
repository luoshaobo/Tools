#!/bin/bash

SRC_ROOT_PATH="$1"
DST_ROOT_PATH="$2"
TMP_FILE_LIST_PATH=`mktemp /tmp/file_list_to_remove_spaces_at_end_of_line.XXXXXXXXXX.txt`
#TMP_FILE_LIST_PATH="/tmp/file_list_to_remove_spaces_at_end_of_line.XXXXXXXXXX.txt"

function usage()
{
    echo "Usage:"
    echo "    `basename $0` <src_root_path> <dst_root_path> [ext_name [ext_name [ext_name ...]]]"
}

function main()
{
    local LINE SRC_FILEPATH SRC_DIR SRC_FILENAME SRC_RELATIVE_DIR DST_DIR DST_FILEPATH
    
    if [ $# -lt 2 ]; then
        usage
        return 1
    fi
    
    echo "\$SRC_ROOT_PATH=$SRC_ROOT_PATH"
    echo "\$DST_ROOT_PATH=$DST_ROOT_PATH"

    > "$TMP_FILE_LIST_PATH"

    if [ ! -d "$SRC_ROOT_PATH" ]; then
        echo "*** ERROR: \$SRC_ROOT_PATH doest not exist: \"$SRC_ROOT_PATH\"" >&2
        return 2
    fi

    if [ ! -d "$DST_ROOT_PATH" ]; then
        echo "*** ERROR: \$DST_ROOT_PATH does not exist: \"$DST_ROOT_PATH\"" >&2
        return 2
    fi

    if [ "$DST_ROOT_PATH" == "$SRC_ROOT_PATH" ]; then
        echo "*** ERROR: \$DST_ROOT_PATH can't equal to \$SRC_ROOT_PATH" >&2
        return 3
    fi
    
    if [ $# -gt 2 ]; then
        shift 2
        while [ $# -gt 0 ]; do
            find "$SRC_ROOT_PATH" -name "*.$1" >> "$TMP_FILE_LIST_PATH"
            shift 1
        done
    else
        find "$SRC_ROOT_PATH" -name "*.c" >> "$TMP_FILE_LIST_PATH"
        find "$SRC_ROOT_PATH" -name "*.cc" >> "$TMP_FILE_LIST_PATH"
        find "$SRC_ROOT_PATH" -name "*.cpp" >> "$TMP_FILE_LIST_PATH"
        find "$SRC_ROOT_PATH" -name "*.h" >> "$TMP_FILE_LIST_PATH"
        find "$SRC_ROOT_PATH" -name "*.hpp" >> "$TMP_FILE_LIST_PATH"
    fi
    
    while read -r LINE; do
        SRC_FILEPATH="$LINE"
        #echo "\$SRC_FILEPATH=$SRC_FILEPATH"
        
        SRC_DIR=`dirname "$SRC_FILEPATH"`
        SRC_FILENAME=`basename "$SRC_FILEPATH"`
        
        SRC_RELATIVE_DIR=${SRC_DIR##$SRC_ROOT_PATH}
        SRC_RELATIVE_DIR=${SRC_RELATIVE_DIR#/}
        #echo "\$SRC_RELATIVE_DIR=$SRC_RELATIVE_DIR"
        
        DST_DIR="$DST_ROOT_PATH/$SRC_RELATIVE_DIR"
        DST_FILEPATH="$DST_DIR/$SRC_FILENAME"
        
        #echo "\$DST_FILEPATH=$DST_FILEPATH"
        
        if [ -n "$SRC_RELATIVE_DIR" ]; then
            echo "convert file: \$SRC_ROOT_PATH/$SRC_RELATIVE_DIR/$SRC_FILENAME ..."
        else
            echo "convert file: \$SRC_ROOT_PATH/$SRC_FILENAME ..."
        fi
        mkdir -p "$DST_DIR"
        sed -e 's/\s\+$//g' "$SRC_FILEPATH" > "$DST_FILEPATH"
        
    done < "$TMP_FILE_LIST_PATH"
}

main $*
rm -f "$TMP_FILE_LIST_PATH"
