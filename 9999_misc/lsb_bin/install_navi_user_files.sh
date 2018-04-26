#!/bin/bash

function usage()
{
    PROG=$(basename "$0")
    
    echo "Usage:"
    echo "    $PROG <src_nav_dir_win_format> [<temp_nav_version>] [<dst_nav_dir_win_format>]"
}

function is_nav_dir()
{
    local DIR_PATH
    
    DIR_PATH="$1"
    
    if [ ! -d "$1" ]; then
        return 1
    fi
    
    if [ ! -f "$1\\Main\\AccessLib\\inc\\entNavigation.h" ]; then
        return 1
    fi
    
    return 0
}

SRC_NAV_DIR="."
DST_NAV_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD\\_NAV"

TEMP_NAV_DIR_ROOT="D:\\casdev\\my_code\\temp\\navi"
TEMP_NAV_VERSION="current"

TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN="d:\\install_navi_user_files.bat"

if [ $# -le 0 ]; then
    usage
    exit 0
fi

if [ $# -ge 1 ]; then
    if [ "$1" == "--help" -o "$1" == "-H" ]; then
        usage
        exit 0
    else
        is_nav_dir "$1"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: the src dir \"$1\" is not a nav main dir!" >&2
            exit 1
        else
            SRC_NAV_DIR="$1"
        fi
    fi
fi

if [ $# -ge 2 ]; then
    TEMP_NAV_VERSION="$2"
fi

if [ $# -ge 3 ]; then
    is_nav_dir "$3"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the dst dir \"$3\" is not a nav main dir!" >&2
        exit 1
    else
        DST_NAV_DIR="$3"
    fi
fi

if [ -L "$DST_NAV_DIR\\Main" ]; then
    echo "INFO: the dir \"$DST_NAV_DIR\\Main\" is already a link and it is not changed!"
    TEMP_NAV_VERSION2=$(ls -l "$DST_NAV_DIR\\Main" | sed -e 's/\(^.*\/\([^/]\+\)\/Main$\)/\2/g')
    if [ "$TEMP_NAV_VERSION2" != "$TEMP_NAV_VERSION" ]; then
        TEMP_NAV_VERSION="$TEMP_NAV_VERSION2"
        echo "*** NOTE: the value TEMP_NAV_VERSION is changed to \"$TEMP_NAV_VERSION\"!"
    fi
else
    if [ ! -d "$TEMP_NAV_DIR_ROOT" ]; then
        mkdir -p "$TEMP_NAV_DIR_ROOT"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: creating dir \"$TEMP_NAV_DIR_ROOT\" is failed!" >&2
            exit 1
        fi
    fi
    rm -fr "$TEMP_NAV_DIR_ROOT\\$TEMP_NAV_VERSION"
    mkdir "$TEMP_NAV_DIR_ROOT\\$TEMP_NAV_VERSION"

    cp -pfr "$DST_NAV_DIR\\Main" "$TEMP_NAV_DIR_ROOT\\$TEMP_NAV_VERSION\\"
    #rm -fr "$DST_NAV_DIR\\Main"
    TEMP_STR=$(basename `mktemp -u`)
    mv -f "$DST_NAV_DIR\\Main" "$DST_NAV_DIR\\Main.$TEMP_STR"
    cmd /c mklink /J "$DST_NAV_DIR\\Main" "$TEMP_NAV_DIR_ROOT\\$TEMP_NAV_VERSION\\Main"
fi

if [ ! -f "$DST_NAV_DIR/Main/AccessLib/src/entNavigation.cpp.orig" ]; then
    mv -f "$DST_NAV_DIR/Main/AccessLib/src/entNavigation.cpp" "$DST_NAV_DIR/Main/AccessLib/src/entNavigation.cpp.orig"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the file \"$DST_NAV_DIR/Main/AccessLib/src/entNavigation.cpp\" doesn't exist!" >2
        exit 1
    fi
fi

if [ ! -f "$DST_NAV_DIR/Main/AccessLib/src/sources.orig" ]; then
    mv -f "$DST_NAV_DIR/Main/AccessLib/src/sources" "$DST_NAV_DIR/Main/AccessLib/src/sources.orig"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the file \"$DST_NAV_DIR/Main/AccessLib/src/sources\" doesn't exist!" >2
        exit 1
    fi
fi

LINKED_FILES=`cat <<EOF
    Main\\AccessLib\\inc\\entNavigation_user.h
    Main\\AccessLib\\src\\entNavigation_user.cpp
    Main\\AccessLib\\src\\TK_DebugServer.cpp
    Main\\AccessLib\\src\\TK_DebugServer.h
    Main\\AccessLib\\src\\TK_sock.cpp
    Main\\AccessLib\\src\\TK_sock.h
    Main\\AccessLib\\src\\TK_Tools.cpp
    Main\\AccessLib\\src\\TK_Tools.h
    Main\\AccessLib\\src\\TK_Bitmap.cpp
    Main\\AccessLib\\src\\TK_Bitmap.h
    Main\\AccessLib\\src\\sources
EOF
`

> "$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN"
for LINKED_FILE in $LINKED_FILES; do
    if [ -L "$DST_NAV_DIR\\$LINKED_FILE" -o -f "$DST_NAV_DIR\\$LINKED_FILE" ]; then
        rm -f "$DST_NAV_DIR\\$LINKED_FILE"
    fi
    
    if [ -f "$DST_NAV_DIR\\$LINKED_FILE" -o -d "$DST_NAV_DIR\\$LINKED_FILE" ]; then
        echo "*** ERROR: \"$DST_NAV_DIR\\$LINKED_FILE\" is a file or a dir!" >&2
        exit 1
    fi
    
    #echo "### cmd /c mklink \"$DST_NAV_DIR\\$LINKED_FILE\" \"$SRC_NAV_DIR\\$LINKED_FILE\""
    echo "cmd /c mklink \"$TEMP_NAV_DIR_ROOT\\$TEMP_NAV_VERSION\\$LINKED_FILE\" \"$SRC_NAV_DIR\\$LINKED_FILE\"" >> "$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN"
done

echo "*** NOTE: please execute the bat file \"$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN\" as amdin!"

exit 0















