#!/bin/bash

FILE_PATH="$1"

function usage
{
    echo "Usage:"
    echo "    `basename $0` <input_file_path>"
    echo ""
}

if [ -z "$FILE_PATH" ]; then
    usage
    exit 1
fi

if [ ! -f "$FILE_PATH" ]; then
    echo "*** ERROR: can't open the file: $FILE_PATH" >&2
    exit 1
fi

LINES=$(cat "$FILE_PATH" | sed -e 's/ /^/g' | sed -e 's/\t/^^/g' | sed -e 's/^$/^/g')

for LINE in $LINES; do
    echo "${LINE}" | grep "guid=" >/dev/null
    if [ $? -eq 0 ]; then
        NEW_GUID=$(uuidgen)
        CMD=`cat <<EOF
echo '\${LINE}' | sed -e 's/guid=\"[0-9A-Za-z]\{8\}-[0-9A-Za-z]\{4\}-[0-9A-Za-z]\{4\}-[0-9A-Za-z]\{4\}-[0-9A-Za-z]\{12\}\"/guid=\\"$NEW_GUID\\"/g' | sed -e 's/\^/ /g' | sed -e 's/^ $//g'
EOF`
        #echo "$CMD"
        eval "$CMD"
    else
        echo "${LINE}" | sed -e 's/\^/ /g' | sed -e 's/^ $//g'
    fi
done
