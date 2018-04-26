#!/bin/bash

DFEET_EXE="d-feet"

if [ -x ~/bin/d-feet ]; then
    DFEET_EXE="$HOME/bin/d-feet"
fi

$DFEET_EXE -a tcp:host=192.168.1.112,port=4444
