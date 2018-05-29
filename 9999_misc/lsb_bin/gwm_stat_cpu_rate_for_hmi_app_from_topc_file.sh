#!/bin/bash

TOPC_FILE_FOR_HMI_APP="_none_topc_file_"
COLUMN_NO_FOR_CPU_RATE="18"

if [ $# -ge 1 ]; then
    TOPC_FILE_FOR_HMI_APP="$1"
fi
if [ $# -ge 2 ]; then
    COLUMN_NO_FOR_CPU_RATE="$2"
fi

cat "$TOPC_FILE_FOR_HMI_APP" | grep "Line " | awk "{ print \$$COLUMN_NO_FOR_CPU_RATE }" | sort -g
