#!/bin/bash

function usage()
{
    echo "Usage:"
    echo "  `basename $0` [-l <lenght>|--length=<length>]"
    echo "Note:"
    echo "  1) the input content is from the stdin."
    echo ""
}

function first_char_to_hex()
{
    echo -n "$1" | od --format=x1 --width=1 | grep "0000000" | awk '{print $2}'
}

if [ $# -ge 1 ]; then
    if [ "$1" == "-H" -o "$1" == "--help" ]; then
        usage
        exit 0
    fi
fi

LENGTH=0
CONTENT=""

ARGV=($(getopt -u -o l:c: -l length:,content: -- "$@"))
for ((i = 0; i < ${#ARGV[@]}; i++)) {
    opt=${ARGV[$i]}
    case $opt in
    -l|--length)
        ((i++))
        LENGTH=${ARGV[$i]}
        ;;
    --)
        ((i++))
        break
        ;;
    esac
}

read CONTENT

for ((i = 0; i < ${#CONTENT}; i++)) {
    CH=${CONTENT:$i:1}
    if [ $i -ne 0 ]; then
        echo -n " "
    fi
    echo -n "`first_char_to_hex $CH`"
}

for ((; i < $LENGTH; i++)) {
    if [ $i -ne 0 ]; then
        echo -n " "
    fi
    echo -n "00"
}

echo ""