#!/bin/bash

ACCESS_LIB_BASE_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD"
OUTPUT_BUILD_ACCESS_LIB_BAT_PATH="D:\\build_access_lib.bat"

get_ACCESS_LIB_DIRS()
{
    (
        cd "$ACCESS_LIB_BASE_DIR" && find -L . -iname "AccessLib" | grep -v ".001" | grep -v ".old" | grep -v ".orig" | grep -v "/OAK/" | sed -e 's/\//\\/g' | sed -e 's/^.\\//'
    )
}

create_output_content()
{
    local ACCESS_LIB_DIRS
    local ACCESS_LIB_DIR
    
    ACCESS_LIB_DIRS="$1"
    
    echo "@echo off"
    echo ""

    for ACCESS_LIB_DIR in $ACCESS_LIB_DIRS; do
        echo "echo ####################################################################################"
        echo "echo ######## {{{ $ACCESS_LIB_DIR"
        echo "cd $ACCESS_LIB_BASE_DIR\\$ACCESS_LIB_DIR"
        echo "build -c"
        echo "echo ######## $ACCESS_LIB_DIR }}}"
        echo ""
    done
    
    echo "@echo on"
}


ACCESS_LIB_DIRS=`get_ACCESS_LIB_DIRS`

for ACCESS_LIB_DIR in $ACCESS_LIB_DIRS; do
    echo "$ACCESS_LIB_DIR"
done

> "$OUTPUT_BUILD_ACCESS_LIB_BAT_PATH"
create_output_content "$ACCESS_LIB_DIRS" >> "$OUTPUT_BUILD_ACCESS_LIB_BAT_PATH"
unix2dos "$OUTPUT_BUILD_ACCESS_LIB_BAT_PATH"
