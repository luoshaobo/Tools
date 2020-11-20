#!/bin/bash

#DFEET_EXE="d-feet"
DFEET_EXE="$HOME/bin/d-feet.001"

#if [ -x ~/bin/d-feet ]; then
#    DFEET_EXE="$HOME/bin/d-feet"
#fi

$DFEET_EXE -a tcp:host=127.0.0.1,port=4444

