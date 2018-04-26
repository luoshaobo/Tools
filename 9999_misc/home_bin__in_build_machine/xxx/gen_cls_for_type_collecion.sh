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
        printf "MY_CLASS(%03d);\n" $I
        I=`expr $I + 1`
    done

    printf "\n"

    printf "typedef TypeCollection<\n"
    I=0
    while [ $I -lt $COUNT ]; do
        printf "    MyClass%03d" $I
        COUNT_1=`expr $COUNT - 1`
        if [ $I -ne $COUNT_1 ]; then
            printf ","
        fi
        printf "\n"
        I=`expr $I + 1`
    done
    printf "> MyTypeCollection;\n"
}
 
if [ $# -lt 1 ]; then
    usage
    return 1
fi

main
