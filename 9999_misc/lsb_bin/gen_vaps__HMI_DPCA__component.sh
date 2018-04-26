#!/bin/bash

ADAPTOR_PATH_PREFIX="D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS"
LOG_FILE_PATH="d:/gen_vaps__HMI_DPCA__component.log"

ADAPTOR_PATHES=`cat <<EOF

EOF`

usage()
{
    echo "Usage:"
    echo "    `basename $0` --all"
    echo "    `basename $0` <adaptor1> [<adaptor2>] [<adaptor3>] ..."
    echo ""
}

ALL_ADAPTORS=""

if [ $# -gt 0 ]; then
    if [ "$1" = "--all" -o "$1" = "-all" -o "$1" = "all" -o "$1" = "-a" -o "$1" = "-A" ]; then
        for ADAPTOR_PATH in $ADAPTOR_PATHES; do
            if [ ! -z "$ADAPTOR_PATH" ]; then
                echo "$ADAPTOR_PATH" | grep -E "^[ \t]*#" >/dev/null 2>&1
                if [ $? -ne 0 ]; then
                    ALL_ADAPTORS="$ALL_ADAPTORS $ADAPTOR_PATH"
                fi
            fi
        done
    elif [ "$1" = "--help" -o "$1" = "-help" -o "$1" = "help" -o "$1" = "-h" -o "$1" = "-H" ]; then
        usage
    else
        ALL_ADAPTORS="$ALL_ADAPTORS $*"
    fi
else
    usage
fi

if [ ! -z "$ALL_ADAPTORS" ]; then
    CMD_LINE=`cat <<EOF
cmd /c "C:\Presagis\VAPS_XT_32\bin\CodeGen.exe" "$ADAPTOR_PATH_PREFIX\HMI2\HMI_DPCA.vrpj" -codegen $ALL_ADAPTORS 2>&1 | tee $LOG_FILE_PATH
EOF`
    echo "$CMD_LINE"
    eval $CMD_LINE
fi
