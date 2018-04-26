#!/bin/bash

usage()
{
    echo "Usage:"
    echo "    `basename $0` <imx_log_file_path>"
    echo ""
}

do_conversion()
{
    local IMX_LOG_FILE_PATH
    
    IMX_LOG_FILE_PATH="$1"
    
    if [ ! -f "$IMX_LOG_FILE_PATH" ]; then
        echo "*** \"$IMX_LOG_FILE_PATH\" is not a valid imx log file path!" >&2
        usage
        return 1
    fi
    
    cat "$IMX_LOG_FILE_PATH" | tr "\'" "\"" | sed -e 's/^[0-9][0-9]\.[0-9][0-9]\.[0-9][0-9][0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9]\.[0-9][0-9][0-9][0-9]  SERIAL #1   | \[None\] \"//g' | sed -e 's/\"$//g' | while read -r LINE; do
        echo -n "$LINE"
    done | sed -e 's/\\n/\n/g' | sed -e 's/\\r//g'
    
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

do_conversion "$1"
