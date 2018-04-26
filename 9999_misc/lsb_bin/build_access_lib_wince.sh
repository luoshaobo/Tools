#!/bin/bash

#
# NOTE:
# 1) This script must be executed in the cygwin command line, and it will generate a bat script to be 
#    executed in WinCE build command line environment.
#

export PATH="D:\cygwin_root\bin:$PATH"

CACHE_FILEPATH="D:\\lsb_bin\\~build_access_lib_wince.cache"
NOT_USE_CACHE=0
CLEAN_BUILD=0

SEARCH_PATHS=`cat <<EOF
D:\casdev\WinCE\public\MMP_PROD
EOF
`

convert_unix_path_to_win_path()
{
    local UNIX_PATH

    UNIX_PATH="$1"
    
    echo "$UNIX_PATH" | sed -e 's/\//\\/g'
}

convert_win_path_to_unix_path()
{
    local UNIX_PATH

    UNIX_PATH="$1"
    
    echo "$UNIX_PATH" | sed -e 's/\\/\//g'
}

is_file_last_modified_before_today()
{
    local RET
    local FILEPATH
    local NOW_YEAR
    local NOW_DAY_IN_YEAR
    local FILE_YEAR
    local FILE_DAY_IN_YEAR
    
    RET=1
    FILEPATH="$1"
    
    if [ ! -f "$FILEPATH" ]; then
        return 0
    fi
        
    NOW_YEAR=$(date "+%-Y")
    NOW_DAY_IN_YEAR=$(date "+%-j")
    
    FILE_YEAR=$(date "+%-Y" -r "$FILEPATH")
    FILE_DAY_IN_YEAR=$(date "+%-j" -r "$FILEPATH")
    
    if [ $NOW_YEAR -gt $FILE_YEAR -o $NOW_DAY_IN_YEAR -gt $FILE_DAY_IN_YEAR ]; then
        RET=0
    fi
    
    return $RET
}

usage()
{
    echo "Usage:"
    echo "    `basename $0` <access_lib_name>"
    echo "    `basename $0` --remove-cache|-r"
    echo "    `basename $0` --not-use-cache|-n"
    echo "    `basename $0` --clean-build|-c"
    echo ""
}

create_cache()
{
    local LINEBUF
    
    > "$CACHE_FILEPATH"
    
    for LINEBUF in $SEARCH_PATHS; do
        if [ -z "$LINEBUF" ]; then
            continue;
        fi
        
        find -L "$LINEBUF" -iname "AccessLib" >> "$CACHE_FILEPATH"
    done
    
    chmod -x "$CACHE_FILEPATH"
}

remove_cache()
{
    rm -f "$CACHE_FILEPATH"
}

FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH=""

find_access_lib_without_cache()
{
    local ACCESS_LIB_FILENAME
    local ACCESS_LIB_PATH

    ACCESS_LIB_FILENAME="$1"
    
    FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH=""
    
    for LINEBUF in $SEARCH_PATHS; do
        ACCESS_LIB_PATH=`find -L "$LINEBUF" -iname "AccessLib" | grep -i "$ACCESS_LIB_FILENAME" | grep -v ".001" | grep -v ".old" | grep -v " - Copy" 2>/dev/null`
        if [ -d "$ACCESS_LIB_PATH" ]; then
            FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH=`convert_unix_path_to_win_path "$ACCESS_LIB_PATH"`
            return 0
        else
            LINE_COUNT=`echo "$ACCESS_LIB_PATH" | wc -l`
            if [ $LINE_COUNT -gt 1 ]; then
                echo "*** Too many line found!" >&2
                echo "<<<---"
                echo "$ACCESS_LIB_PATH" >&2
                echo "--->>>"
                return 1
            fi
        fi
    done

    return 1
}

find_access_lib_with_cache()
{
    local ACCESS_LIB_FILENAME
    local ACCESS_LIB_PATH
    
    ACCESS_LIB_FILENAME="$1"
        
    FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH=""
    
    ACCESS_LIB_PATH=`cat "$CACHE_FILEPATH" | grep -i "$ACCESS_LIB_FILENAME" | grep -v ".001" | grep -v ".old" | grep -v " - Copy" 2>/dev/null`
    if [ -d "$ACCESS_LIB_PATH" ]; then
        FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH=`convert_unix_path_to_win_path $ACCESS_LIB_PATH`
        return 0
    else
        LINE_COUNT=`echo "$ACCESS_LIB_PATH" | wc -l`
        if [ $LINE_COUNT -gt 1 ]; then
            echo "*** Too many line found!" >&2
            echo "<<<---"
            echo "$ACCESS_LIB_PATH" >&2
            echo "--->>>"
            return 1
        fi
    fi

    return 1
}

find_access_lib()
{
    if [ $NOT_USE_CACHE -eq 0 -a -f "$CACHE_FILEPATH" ]; then
        find_access_lib_with_cache $*
    else
        find_access_lib_without_cache $*
    fi
}

BUILD_ACCESS_LIB_TEMP_BAT_FILEPATH="d:\build_access_lib_temp.bat"

build_access_lib()
{
    local SRC_DIR
    local SRC_DIR_WIN
    local CLEAN_BUILD_OPT
    
    SRC_DIR="$FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH"
    SRC_DIR_WIN=`convert_unix_path_to_win_path "$SRC_DIR"`
    CLEAN_BUILD_OPT=""
    
    if [ $CLEAN_BUILD -eq 1 ]; then
        CLEAN_BUILD_OPT="-c"
    fi
    
    cat > "$BUILD_ACCESS_LIB_TEMP_BAT_FILEPATH" <<EOF
cd /d "$SRC_DIR_WIN"
build $CLEAN_BUILD_OPT
EOF
    unix2dos "$BUILD_ACCESS_LIB_TEMP_BAT_FILEPATH"

    echo "========================================================================================="
    cat  "$BUILD_ACCESS_LIB_TEMP_BAT_FILEPATH"
    
    echo "========================================================================================="
}

get_pattern_from_name()
{
    local NAME
    local RET_PATTERN
    
    NAME="$1"
    RET_PATTERN="$NAME"
    
    echo "$NAME" | grep -i "nav.orig" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        RET_PATTERN="/_nav/main.orig/"
        echo "$RET_PATTERN"
        return 0
    fi
    
    echo "$NAME" | grep -i "nav" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        RET_PATTERN="/_nav/main/"
        echo "$RET_PATTERN"
        return 0
    fi
    
    echo "$NAME" | grep -i "av.orig" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        RET_PATTERN="/_av/main.orig/"
        echo "$RET_PATTERN"
        return 0
    fi
    
    echo "$NAME" | grep -i "av" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        RET_PATTERN="/_av/main/"
        echo "$RET_PATTERN"
        return 0
    fi
    
    echo "$RET_PATTERN"
    return 0
}

ACCESS_LIBS=""

while [ 1 -eq 1 ]; do
    if [ $# -ge 1 ]; then
        if [ "$1" == "--help" -o "$1" == "-help" -o "$1" == "-h" -o "$1" == "-H" ]; then
            usage
            exit 0
        elif [ "$1" == "--remove-cache" -o "$1" == "-r" ]; then
            remove_cache
            exit 0
        elif [ "$1" == "--not-use-cache" -o "$1" == "-n" ]; then
            NOT_USE_CACHE=1
            shift
        elif [ "$1" == "--clean-build" -o "$1" == "-c" ]; then
            CLEAN_BUILD=1
            shift
        else
            ACCESS_LIBS="$*"
            break
        fi
    else
        usage
        exit 0
    fi
done

if is_file_last_modified_before_today "$CACHE_FILEPATH"; then
    echo "*** Old cache file removed!"
    rm -f "$CACHE_FILEPATH"
fi

if [ ! -f "$CACHE_FILEPATH" ]; then
    create_cache
fi

for ACCESS_LIB in $ACCESS_LIBS; do
    echo "========================================================================================="
    
    if [ -z "$ACCESS_LIB" ]; then
        continue
    fi
    
    ACCESS_LIB_PATTERN=`get_pattern_from_name "$ACCESS_LIB"`
    ACCESS_LIB="$ACCESS_LIB_PATTERN"
    
    find_access_lib "$ACCESS_LIB"
    if [ $? -ne 0 ]; then
        echo "The pathname of $ACCESS_LIB is: $FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH"
        echo "*** Failed to find the relevant directories and files for $ACCESS_LIB!" >&2
        exit 1
    fi
    
    echo "The pathname of $ACCESS_LIB is: $FIND_ACCESS_LIB_RETURN_ACCESS_LIB_PATH"
    build_access_lib
done
