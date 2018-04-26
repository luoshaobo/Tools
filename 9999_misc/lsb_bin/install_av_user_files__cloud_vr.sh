#!/bin/bash

function usage()
{
    PROG=$(basename "$0")
    
    echo "Usage:"
    echo "    $PROG <src_av_dir_win_format> [<temp_av_version>] [<dst_av_dir_win_format>]"
}

function is_av_dir()
{
    local DIR_PATH
    
    DIR_PATH="$1"
    
    if [ ! -d "$1" ]; then
        return 1
    fi
    
    if [ ! -f "$1\\Main\\AccessLib\\src\\entIAvSpeech.cpp" -a ! -f "$1\\Main\\AccessLib\\src\\av_access_lib_src_dir" ]; then
        return 1
    fi
    
    return 0
}

SRC_AV_DIR="."
DST_AV_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD\\_AV"

TEMP_AV_DIR_ROOT="D:\\casdev\\my_code\\temp\\av"
TEMP_AV_VERSION="current"

TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN="d:\\install_av_user_files.bat"

if [ $# -le 0 ]; then
    usage
    exit 0
fi

if [ $# -ge 1 ]; then
    if [ "$1" == "--help" -o "$1" == "-H" ]; then
        usage
        exit 0
    else
        is_av_dir "$1"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: the src dir \"$1\" is not a av main dir!" >&2
            exit 1
        else
            SRC_AV_DIR="$1"
        fi
    fi
fi

if [ $# -ge 2 ]; then
    TEMP_AV_VERSION="$2"
fi

if [ $# -ge 3 ]; then
    is_av_dir "$3"
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the dst dir \"$3\" is not a av main dir!" >&2
        exit 1
    else
        DST_AV_DIR="$3"
    fi
fi

if [ -L "$DST_AV_DIR\\Main" ]; then
    echo "INFO: the dir \"$DST_AV_DIR\\Main\" is already a link and it is not changed!"
    TEMP_AV_VERSION2=$(ls -l "$DST_AV_DIR\\Main" | sed -e 's/\(^.*\/\([^/]\+\)\/Main$\)/\2/g')
    if [ "$TEMP_AV_VERSION2" != "$TEMP_AV_VERSION" ]; then
        TEMP_AV_VERSION="$TEMP_AV_VERSION2"
        echo "*** NOTE: the value TEMP_AV_VERSION is changed to \"$TEMP_AV_VERSION\"!"
    fi
else
    if [ ! -d "$TEMP_AV_DIR_ROOT" ]; then
        mkdir -p "$TEMP_AV_DIR_ROOT"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: creating dir \"$TEMP_AV_DIR_ROOT\" is failed!" >&2
            exit 1
        fi
    fi
    rm -fr "$TEMP_AV_DIR_ROOT\\$TEMP_AV_VERSION"
    mkdir "$TEMP_AV_DIR_ROOT\\$TEMP_AV_VERSION"

    cp -pfr "$DST_AV_DIR\\Main" "$TEMP_AV_DIR_ROOT\\$TEMP_AV_VERSION\\"
    #rm -fr "$DST_AV_DIR\\Main"
    TEMP_STR=$(basename `mktemp -u`)
    mv -f "$DST_AV_DIR\\Main" "$DST_AV_DIR\\Main.$TEMP_STR"
    cmd /c mklink /J "$DST_AV_DIR\\Main" "$TEMP_AV_DIR_ROOT\\$TEMP_AV_VERSION\\Main"
fi

SRC_FILES=`cat <<EOF
entIAvAasRvc.cpp
entIAvAudio.cpp
entIAvAudioVideo.cpp
entIAvCitypark.cpp
entIAvSource.cpp
entIAvSpeech.cpp
entIAvVideo.cpp
entIAvCloudVr.cpp
EOF
`
for SRC_FILE in $SRC_FILES; do
    if [ ! -f "$DST_AV_DIR/Main/AccessLib/src/$SRC_FILE.orig" ]; then
        mv -f "$DST_AV_DIR/Main/AccessLib/src/$SRC_FILE" "$DST_AV_DIR/Main/AccessLib/src/$SRC_FILE.orig"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: the file \"$DST_AV_DIR/Main/AccessLib/src/$SRC_FILE\" doesn't exist!" >2
            exit 1
        fi
    fi
done

LINKED_FILES=`cat <<EOF
    Main\\AccessLib\\src\\entIAvAudioVideo_user.h
    Main\\AccessLib\\src\\entIAvAudioVideo_user.cpp
    Main\\AccessLib\\src\\sources
EOF
`

> "$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN"
for LINKED_FILE in $LINKED_FILES; do
    if [ -L "$DST_AV_DIR\\$LINKED_FILE" -o -f "$DST_AV_DIR\\$LINKED_FILE" ]; then
        rm -f "$DST_AV_DIR\\$LINKED_FILE"
    fi
    
    if [ -f "$DST_AV_DIR\\$LINKED_FILE" -o -d "$DST_AV_DIR\\$LINKED_FILE" ]; then
        echo "*** ERROR: \"$DST_AV_DIR\\$LINKED_FILE\" is a file or a dir!" >&2
        exit 1
    fi
    
    #echo "### cmd /c mklink \"$DST_AV_DIR\\$LINKED_FILE\" \"$SRC_AV_DIR\\$LINKED_FILE\""
    echo "cmd /c mklink \"$TEMP_AV_DIR_ROOT\\$TEMP_AV_VERSION\\$LINKED_FILE\" \"$SRC_AV_DIR\\$LINKED_FILE\"" >> "$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN"
done

echo "*** NOTE: please execute the bat file \"$TEMP_BAT_PATH_TO_BE_EXEC_AS_ADMIN\" as amdin!"

exit 0















