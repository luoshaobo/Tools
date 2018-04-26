#!/bin/bash

function subs_files
{
    for FILE in $FILES; do
        FILE_OUT="${FILE}.new"
        echo "bash subs_uudi.sh \"$FILE\" > \"$FILE_OUT\""
        bash subs_uudi.sh "$FILE" > "$FILE_OUT"
        if [ $? -ne 0 ]; then
            echo "*** ERROR: failed!"
            exit 1
        fi
        mv -f "$FILE" "${FILE}.old"
        mv -f "$FILE_OUT" "$FILE"
    done
}

FILES=$(find -name "*.vcls")
subs_files $FILES

FILES=$(find -name "*.vimp")
subs_files $FILES
