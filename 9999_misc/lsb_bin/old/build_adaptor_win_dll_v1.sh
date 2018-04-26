#!/bin/bash

SEARCH_PATHS=`cat <<EOF
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Controls
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Infrastructure
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Infrastructure
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Toolbox_DPCA
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Controls
EOF
`

usage()
{
    echo "Usage:"
    echo "    `basename $0` <name>.vtpy|<name>[.vcls]"
    echo ""
}

FIND_ADAPTOR_RETURN_ADAPTOR_PATH=""
FIND_ADAPTOR_RETURN_VCPROJ_PATH=""

find_adaptor()
{
    local ADAPTOR_PATH
    local ADAPTOR_DIR
    
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
        fi
    done

    return 1
}

build_adaptor()
{
    local VCPROJ_DIR
    local VCPROJ_FILENAME
    
    VCPROJ_DIR=`dirname "$FIND_ADAPTOR_RETURN_VCPROJ_PATH"`
    VCPROJ_FILENAME=`basename "$FIND_ADAPTOR_RETURN_VCPROJ_PATH"`
    
    CMDLINE=`cat <<EOF
(cd "$VCPROJ_DIR" && vcbuild $VCPROJ_FILENAME release)
EOF
`
    echo "$CMDLINE"
    eval "$CMDLINE"
}

ADAPTORS=""

if [ $# -ge 1 ]; then
    if [ "$1" == "--help" -o "$1" == "-h" ]; then
        usage
        exit 0
    else
        ADAPTORS="$*"
    fi
else
    usage
    exit 0
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
        echo "The pathname of $ADAPTOR is $FIND_ADAPTOR_RETURN_ADAPTOR_PATH"
        echo "The pathname of *.vcproj for $ADAPTOR is $FIND_ADAPTOR_RETURN_VCPROJ_PATH"
        echo "*** Failed to find the relevant directories and files for $ADAPTOR!" >&2
        exit 1
    fi
    
    echo "The pathname of $ADAPTOR is $FIND_ADAPTOR_RETURN_ADAPTOR_PATH"
    echo "The pathname of *.vcproj for $ADAPTOR is $FIND_ADAPTOR_RETURN_VCPROJ_PATH"
    build_adaptor
done
