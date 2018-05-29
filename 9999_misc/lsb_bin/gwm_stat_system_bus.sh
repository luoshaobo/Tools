#!/bin/bash

LOG_FILE_PATH="_unknown_log_file_path_"

if [ $# -ge 1 ]; then
    LOG_FILE_PATH="$1"
fi

function stat_on_file()
{
    DLT_CONTEXT_ID="$1"

    cat "$LOG_FILE_PATH" | grep "SBUS $DLT_CONTEXT_ID log info verbose 1 Overall Bus Load:" > "./system_bus_result_${DLT_CONTEXT_ID}.txt"
    cat "./system_bus_result_${DLT_CONTEXT_ID}.txt" | awk '{ print $16 }' | sort > "./system_bus_sorted_result_${DLT_CONTEXT_ID}.txt"
}

stat_on_file "SUM"
stat_on_file "G3D"
stat_on_file "G2D1"
stat_on_file "G2D2"
stat_on_file "ARM"
stat_on_file "DSP1"
stat_on_file "VPU"
stat_on_file "USB"
