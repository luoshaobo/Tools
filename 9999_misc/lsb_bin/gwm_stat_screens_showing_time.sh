#!/bin/bash

PATH_INPUT_LOG="$1"
gwm_pick_out_screen_showing_log_lines.sh "$PATH_INPUT_LOG" > ./Temp_AppData.py

export PYTHONPATH=`pwd`:$PYTHONPATH
gwm_calc_screens_time.py

#rm Temp_AppData.py Temp_AppData.pyc
