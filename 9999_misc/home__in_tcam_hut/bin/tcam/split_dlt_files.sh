#!/bin/bash

while [ $# -ge 1 ]; do
    DLT_FILE_PATH="$1"
    
    echo "################################################################################"
    echo "### $DLT_FILE_PATH"
    split_single_dlt_file.sh "$DLT_FILE_PATH"

    shift
done
