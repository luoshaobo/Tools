#!/bin/bash

COUNT="$1"

function usage
{
    echo "Usage:"
    echo "  `basename $0` <count>"
}

function main
{
    local I
    local COUNT_1
    
    I=0
    while [ $I -lt $COUNT ]; do
        printf "MY_SCREEN(%03d);\n" $I
        I=`expr $I + 1`
    done

    printf "\n"

    printf "#define SCREEN_POINTERS \\"
    printf "\n"
    I=0
    while [ $I -lt $COUNT ]; do
        printf "    (MyScreen%03d *)0" $I
        COUNT_1=`expr $COUNT - 1`
        if [ $I -ne $COUNT_1 ]; then
            printf ", \\"
        fi
        printf "\n"
        I=`expr $I + 1`
    done
    printf "\n"
    printf "#define SCREEN_POINTER_COUNT $COUNT\n"
}
 
if [ $# -lt 1 ]; then
    usage
    return 1
fi

main
