#!/bin/bash

DFEET_EXE="d-feet"

if [ -x ~/bin/d-feet ]; then
    DFEET_EXE="$HOME/bin/d-feet"
fi

$DFEET_EXE -a tcp:host=127.0.0.1,port=4444

