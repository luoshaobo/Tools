#!/bin/bash

if [ $# -ge 1 ]; then
    if [ "$1" == "-f" ]; then
        adb kill-server
        shift 1
    fi
fi

adb shell $*
