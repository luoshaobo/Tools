#!/bin/bash

#
# NOTE:
# 1) This script must be executed in the cygwin command line with PC vs2008 vcbuild environment.
#

ADAPTOR_PATH_PREFIX="D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS"
CACHE_FILEPATH="D:\\lsb_bin\\~build_adaptor_win_dll.cache"
NOT_USE_CACHE=0
CLEAN_BUILD=0

SEARCH_PATHS=`cat <<EOF
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Infrastructure
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Controls
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Infrastructure
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Toolbox_DPCA
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Controls
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
    echo "    `basename $0` <name>.vtpy|<name>[.vcls]"
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
        
        find $LINEBUF | tr '/' '\\' >> "$CACHE_FILEPATH"
    done
    
    chmod -x "$CACHE_FILEPATH"
}

remove_cache()
{
    rm -f "$CACHE_FILEPATH"
}

FIND_ADAPTOR_RETURN_ADAPTOR_PATH=""
FIND_ADAPTOR_RETURN_VCPROJ_PATH=""

find_adaptor_without_cache()
{
    local ADAPTOR_PATH
    local ADAPTOR_DIR
    local ADAPTOR_NAME
    local ADAPTOR_NAME_WITHOUT_EXT
    local VCPROJ_PATH
    
    FIND_ADAPTOR_RETURN_ADAPTOR_PATH=""
    FIND_ADAPTOR_RETURN_VCPROJ_PATH=""
    
    for LINEBUF in $SEARCH_PATHS; do
        ADAPTOR_PATH=`find -L "$LINEBUF" -iname "$1" | grep -i "$1" | grep -v ".001" | grep -v ".old" | grep -v " - Copy" 2>/dev/null`
        if [ -f "$ADAPTOR_PATH" ]; then
            FIND_ADAPTOR_RETURN_ADAPTOR_PATH="$ADAPTOR_PATH"
            ADAPTOR_DIR=`dirname "$ADAPTOR_PATH"`
            ADAPTOR_NAME=`basename "$ADAPTOR_PATH"`
            ADAPTOR_NAME_WITHOUT_EXT=${ADAPTOR_NAME%.*}
            
            VCPROJ_PATH=`find -L "$ADAPTOR_DIR" -iname "*$ADAPTOR_NAME_WITHOUT_EXT.vcproj" | grep -i "$ADAPTOR_NAME_WITHOUT_EXT.vcproj" 2>/dev/null`
            if [ ! -f "$VCPROJ_PATH" ]; then
                return 1
            else
                FIND_ADAPTOR_RETURN_VCPROJ_PATH="$VCPROJ_PATH"
                return 0
            fi
        else
            LINE_COUNT=`echo "$ADAPTOR_PATH" | wc -l`
            if [ $LINE_COUNT -gt 1 ]; then
                echo "*** Too many line found!" >&2
                echo "<<<---"
                echo "$ADAPTOR_PATH" >&2
                echo "--->>>"
            fi
        fi
    done

    return 1
}

find_adaptor_with_cache()
{
    local ADAPTOR_FILENAME
    local ADAPTOR_PATH
    local ADAPTOR_DIR
    local ADAPTOR_NAME
    local ADAPTOR_NAME_WITHOUT_EXT
    
    ADAPTOR_FILENAME="$1"
        
    FIND_ADAPTOR_RETURN_ADAPTOR_PATH=""
    FIND_ADAPTOR_RETURN_VCPROJ_PATH=""
    
    ADAPTOR_FILENAME_PATTERN="\\\\"
    ADAPTOR_FILENAME_PATTERN="$ADAPTOR_FILENAME_PATTERN$ADAPTOR_FILENAME"
    ADAPTOR_PATH=`cat "$CACHE_FILEPATH" | grep -i "$ADAPTOR_FILENAME_PATTERN" | grep -v ".001" | grep -v ".old" | grep -v " - Copy" | head -n 1 2>/dev/null`
    echo "$ADAPTOR_PATH"
    if [ -f "$ADAPTOR_PATH" ]; then
        FIND_ADAPTOR_RETURN_ADAPTOR_PATH="$ADAPTOR_PATH"
        ADAPTOR_DIR=`dirname "$ADAPTOR_PATH"`
        ADAPTOR_NAME=`basename "$ADAPTOR_PATH"`
        ADAPTOR_NAME_WITHOUT_EXT=${ADAPTOR_NAME%.*}
        
        VCPROJ_PATH=`find -L "$ADAPTOR_DIR" -iname "*$ADAPTOR_NAME_WITHOUT_EXT.vcproj" | grep -i "$ADAPTOR_NAME_WITHOUT_EXT.vcproj" 2>/dev/null`
        if [ ! -f "$VCPROJ_PATH" ]; then
            return 1
        else
            FIND_ADAPTOR_RETURN_VCPROJ_PATH="$VCPROJ_PATH"
            return 0
        fi
    else
        LINE_COUNT=`echo "$ADAPTOR_PATH" | wc -l`
        if [ $LINE_COUNT -gt 1 ]; then
            echo "*** Too many line found!" >&2
            echo "<<<---"
            echo "$ADAPTOR_PATH" >&2
            echo "--->>>"
        fi
    fi

    return 1
}

find_adaptor()
{
    if [ $NOT_USE_CACHE -eq 0 -a -f "$CACHE_FILEPATH" ]; then
        find_adaptor_with_cache $*
    else
        find_adaptor_without_cache $*
    fi
}

build_adaptor()
{
    local VCPROJ_DIR
    local VCPROJ_FILENAME
    local CMDLINE
    local VCPROJ_DIR_WIN
    local VCPROJ_DIR_UNIX
    
    VCPROJ_DIR=`dirname "$FIND_ADAPTOR_RETURN_VCPROJ_PATH"`
    VCPROJ_FILENAME=`basename "$FIND_ADAPTOR_RETURN_VCPROJ_PATH"`
    VCPROJ_DIR_WIN=`convert_unix_path_to_win_path "$VCPROJ_DIR"`
    VCPROJ_DIR_UNIX=`convert_win_path_to_unix_path "$VCPROJ_DIR"`
    
    DELETE_RELEASE_FILES_CMDLINE=""
    if [ $CLEAN_BUILD -eq 1 ]; then
        CLEAN_BUILD_OPT="rm -f $VCPROJ_DIR_UNIX/release/*;"
    fi
    
    CMDLINE=`cat <<EOF
$CLEAN_BUILD_OPT
(cd "$VCPROJ_DIR_WIN" && vcbuild $VCPROJ_FILENAME release)
EOF
`
    echo "$CMDLINE"
    eval "$CMDLINE"
}

ADAPTORS=""

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
            ADAPTORS="$*"
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

for ADAPTOR in $ADAPTORS; do
    echo "========================================================================================="
    
    LAST_CHAR=`expr substr "$ADAPTOR" ${#ADAPTOR} 1`
    if [ "$LAST_CHAR" == "~" ]; then
        FIRST_CHARS=`expr substr "$ADAPTOR" 1 $((${#ADAPTOR}-1))`
        ADAPTOR="${FIRST_CHARS}Adaptor.vcls"
    fi
    
    DOT_POS=`expr index "$ADAPTOR" '.'`
    if [ "$DOT_POS" -eq 0 ]; then               # no extention name
        ADAPTOR="$ADAPTOR.vcls"
    fi
    
    find_adaptor "$ADAPTOR"
    if [ $? -ne 0 ]; then
        echo "The pathname of $ADAPTOR is: $FIND_ADAPTOR_RETURN_ADAPTOR_PATH"
        echo "The pathname of *${ADAPTOR%.*}.vcproj for $ADAPTOR is: $FIND_ADAPTOR_RETURN_VCPROJ_PATH"
        echo "*** Failed to find the relevant directories and files for $ADAPTOR!" >&2
        exit 1
    fi
    
    echo "The pathname of $ADAPTOR is: $FIND_ADAPTOR_RETURN_ADAPTOR_PATH"
    echo "The pathname of *${ADAPTOR%.*}.vcproj for $ADAPTOR is: $FIND_ADAPTOR_RETURN_VCPROJ_PATH"
    build_adaptor
done
