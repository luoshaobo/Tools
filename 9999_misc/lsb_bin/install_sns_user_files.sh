#!/bin/bash

function usage()
{
    PROG=$(basename "$0")
    
    echo "Usage:"
    echo "    $PROG <src_sns_dir_win_format> [<temp_sns_version>] [<dst_sns_dir_win_format>]"
}

function is_sns_dir()
{
    local DIR_PATH
    
    DIR_PATH="$1"
    
    if [ ! -d "$1" ]; then
        return 1
    fi
    
    if [ ! -f "$1\\AccessLib\\src\\entISnsAccessLib.cpp" ]; then
        return 1
    fi
    
    return 0
}

SRC_SNS_DIR="."
DST_SNS_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD\\_NAV\\NavSNS\\Main"

TEMP_SNS_DIR_ROOT="D:\\casdev\\my_code\\temp\\sns"
TEMP_SNS_VERSION="current"

TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN="d:\\install_sns_user_files.bat"

if [ $# -le 0 ]; then
    usage
    exit 0
fi

if [ $# -ge 1 ]; then
    if [ "$1" == "--help" -o "$1" == "-H" ]; then
        usage
        exit 0
    else
        is_sns_dir "$1"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: the src dir \"$1\" is not a sns main dir!" >&2
            exit 1
        else
            SRC_SNS_DIR="$1"
        fi
    fi
fi

if [ $# -ge 2 ]; then
    TEMP_SNS_VERSION="$2"
fi

if [ $# -ge 3 ]; then
    is_sns_dir "$3"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the dst dir \"$3\" is not a sns main dir!" >&2
        exit 1
    else
        DST_SNS_DIR="$3"
    fi
fi

if [ -L "$DST_SNS_DIR\\AccessLib" ]; then
    echo "INFO: the dir \"$DST_SNS_DIR\\AccessLib\" is already a link and it is not changed!"
    TEMP_SNS_VERSION2=$(ls -l "$DST_SNS_DIR\\AccessLib" | sed -e 's/\(^.*\/\([^/]\+\)\/AccessLib$\)/\2/g')
    if [ "$TEMP_SNS_VERSION2" != "$TEMP_SNS_VERSION" ]; then
        TEMP_SNS_VERSION="$TEMP_SNS_VERSION2"
        echo "*** NOTE: the value TEMP_SNS_VERSION is changed to \"$TEMP_SNS_VERSION\"!"
    fi
else
    if [ ! -d "$TEMP_SNS_DIR_ROOT" ]; then
        mkdir -p "$TEMP_SNS_DIR_ROOT"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: creating dir \"$TEMP_SNS_DIR_ROOT\" is failed!" >&2
            exit 1
        fi
    fi
    rm -fr "$TEMP_SNS_DIR_ROOT\\$TEMP_SNS_VERSION"
    mkdir "$TEMP_SNS_DIR_ROOT\\$TEMP_SNS_VERSION"

    cp -pfr "$DST_SNS_DIR\\AccessLib" "$TEMP_SNS_DIR_ROOT\\$TEMP_SNS_VERSION\\"
    #rm -fr "$DST_SNS_DIR\\AccessLib"
    TEMP_STR=$(basename `mktemp -u`)
    mv -f "$DST_SNS_DIR\\AccessLib" "$DST_SNS_DIR\\AccessLib.$TEMP_STR"
    cmd /c mklink /J "$DST_SNS_DIR\\AccessLib" "$TEMP_SNS_DIR_ROOT\\$TEMP_SNS_VERSION\\AccessLib"
fi

if [ ! -f "$DST_SNS_DIR/AccessLib/src/entISnsAccessLib.cpp.orig" ]; then
    mv -f "$DST_SNS_DIR/AccessLib/src/entISnsAccessLib.cpp" "$DST_SNS_DIR/AccessLib/src/entISnsAccessLib.cpp.orig"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the file \"$DST_SNS_DIR/AccessLib/src/entISnsAccessLib.cpp\" doesn't exist!" >2
        exit 1
    fi
fi

if [ ! -f "$DST_SNS_DIR/AccessLib/src/sources.orig" ]; then
    mv -f "$DST_SNS_DIR/AccessLib/src/sources" "$DST_SNS_DIR/AccessLib/src/sources.orig"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the file \"$DST_SNS_DIR/AccessLib/src/sources\" doesn't exist!" >2
        exit 1
    fi
fi

LINKED_FILES=`cat <<EOF
    AccessLib\\src\\entISnsAccessLib_user.h
    AccessLib\\src\\entISnsAccessLib_user.cpp
    AccessLib\\src\\sources
EOF
`

> "$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN"
for LINKED_FILE in $LINKED_FILES; do
    if [ -L "$DST_SNS_DIR\\$LINKED_FILE" -o -f "$DST_SNS_DIR\\$LINKED_FILE" ]; then
        rm -f "$DST_SNS_DIR\\$LINKED_FILE"
    fi
    
    if [ -f "$DST_SNS_DIR\\$LINKED_FILE" -o -d "$DST_SNS_DIR\\$LINKED_FILE" ]; then
        echo "*** ERROR: \"$DST_SNS_DIR\\$LINKED_FILE\" is a file or a dir!" >&2
        exit 1
    fi
    
    #echo "### cmd /c mklink \"$DST_SNS_DIR\\$LINKED_FILE\" \"$SRC_SNS_DIR\\$LINKED_FILE\""
    echo "cmd /c mklink \"$TEMP_SNS_DIR_ROOT\\$TEMP_SNS_VERSION\\$LINKED_FILE\" \"$SRC_SNS_DIR\\$LINKED_FILE\"" >> "$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN"
done

echo "*** NOTE: please execute the bat file \"$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN\" as amdin!"

exit 0















