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

    printf "core::ScreenClassInfo screenClassInfo[] = {\n"
    I=0
    while [ $I -lt $COUNT ]; do
        printf "    { typeid(MyClass%03u), %u, [](){ return new MyClass%03u(); } },\n" $I $I $I
        I=`expr $I + 1`
    done
    printf "};\n"
    printf "std::size_t screenClassCount = sizeof(screenClassInfo) / sizeof(screenClassInfo[0]);\n"
}
 
if [ $# -lt 1 ]; then
    usage
    return 1
fi

main
