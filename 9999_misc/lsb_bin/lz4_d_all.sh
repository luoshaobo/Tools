#!/bin/bash

LZ4_FILES=`ls *.lz4`
for LZ4_FILE in $LZ4_FILES; do
    if [ ! -z "$LZ4_FILE" ]; then
        lz4 -d "$LZ4_FILE"
    fi
done
