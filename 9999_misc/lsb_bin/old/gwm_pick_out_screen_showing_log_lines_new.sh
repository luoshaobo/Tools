#!/bin/bash

PATH_LOG="$1"

function usage
{
    echo "Usage:"
    echo "    `basename $0` <path_of_text_log_from_dlt>"
}

function main
{
    echo "arrItems = ["
    cat "$PATH_LOG" | grep -e "QmlDrawable::show\|QSG frameSwapped" | awk ' { print "    { \"time\": " $4 ", \"text\": \"", $16 "\" }, " } '
    echo "];"
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

main
