#!/bin/bash

for FILE in `find -name "*.htm"`; do
    echo "==========================="
    echo "$FILE"
    cat "$FILE" | grep "TCAM_SyRD_" | wc -l
done
