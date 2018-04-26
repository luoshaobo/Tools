#!/bin/bash

WINCE_TARGET__WIN="$1"

WINCE_LINK__WIN='D:\casdev\WinCE'
WINCE_LINK__UNIX="/cygdrive/d/casdev/WinCE"

# WINCE_LINK__WIN='D:\temp\WinCE'
# WINCE_LINK__UNIX="/cygdrive/d/temp/WinCE"

function usage()
{
    echo "Usage:"
    echo "    `basename $0` <wince_target_dir__win>"
    echo "    "
}

if [ $# -lt 1 -o -z "$WINCE_TARGET__WIN" ]; then
    usage
    exit 1
fi

if [ $# -ge 1 -a "$1" == "-L" ]; then
    cmd /c dir 'd:\casdev'
    exit 0
fi

echo "rm -f $WINCE_LINK__UNIX"
rm -f $WINCE_LINK__UNIX
if [ $? -ne 0 ]; then
    echo "*** FATAL ERROR: can't remove $WINCE_LINK__UNIX" >&2
    exit 1
fi

echo "cmd /C mklink /J $WINCE_LINK__WIN \"$WINCE_TARGET__WIN\""
cmd /C mklink /J $WINCE_LINK__WIN "$WINCE_TARGET__WIN"
if [ $? -ne 0 ]; then
    echo "*** FATAL ERROR: can't create windows link to $WINCE_TARGET__WIN" >&2
    exit 1
fi

exit 0
