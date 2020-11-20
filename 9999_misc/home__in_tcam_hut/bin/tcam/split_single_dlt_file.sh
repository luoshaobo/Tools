#!/bin/bash

# sample life cylce splitting flag lines
cat >/dev/null <<EOF
$ dlt-convert -a lifecycle_0001.dlt | grep 'DC1- control response N 1 \[service(' 
4      2020/11/09 18:21:46.425358     455363 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 02 00 00 00 00]
1381   2020/11/09 18:21:46.640785     455279 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 01 00 00 00 00]
148463 2020/11/09 18:34:43.449258     267244 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 02 00 00 00 00]
165147 2020/11/09 18:40:04.460606    1437348 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 02 00 00 00 00]
167961 2020/11/09 18:40:04.544383    1437194 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 01 00 00 00 00]
851474 2020/11/09 18:49:32.746054     245133 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 02 00 00 00 00]
869586 2020/11/09 18:54:55.649536    1451941 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 02 00 00 00 00]
946395 2020/11/09 19:04:15.466937     235552 000 TCAM DA1- DC1- control response N 1 [service(3842), ok, 02 00 00 00 00]
EOF

function usage
{
    echo "Usage:"
    echo "  `basename $0` <input_dlt_file_path>"
    echo ""
}

if [ $# -ne 1 ]; then
    usage
    exit 1
fi

if [ ! -r "$1" ]; then
    echo "*** ERROR: can't read file: $1" >&2
    exit 2
fi

INPUT_FILE_PATH="$1"
INPUT_FILE_BASENAME=`basename "${INPUT_FILE_PATH}"`
OUTPUT_FILE_PATH_PATTERN="$INPUT_FILE_BASENAME.%04u.dlt"

#*** NOTE***: if ($4 < 1000000) to ignore some fake life cycles
LIFECYCLE_INDECS=`dlt-convert -a "$INPUT_FILE_PATH" | grep 'DC1- control response N 1 \[service(' | awk ' { if ($4 < 1000000) { print $1 } } '`

START_INDEX=0
END_INDEX=0
I=0
for INDEX in $LIFECYCLE_INDECS; do
    DIFF_END_INDEX=$((INDEX - END_INDEX))
    
    #*** NOTE***: ignore some duplicated life cycles
    if [ "$DIFF_END_INDEX" -lt 10000 ]; then
        continue
    fi

    START_INDEX=$END_INDEX
    END_INDEX=$INDEX
    
    END_INDEX_LESS_THAN_1=$((END_INDEX - 1))
    
    OUTPUT_FILE_PATH=`printf "$OUTPUT_FILE_PATH_PATTERN" $I`
    echo "=== $OUTPUT_FILE_PATH === START_INDEX: $START_INDEX ~ END_INDEX: $END_INDEX_LESS_THAN_1"
    
    dlt-convert -b $START_INDEX -e $END_INDEX_LESS_THAN_1 -o "$OUTPUT_FILE_PATH" "$INPUT_FILE_PATH"
    
    I=$((I+1))
done

START_INDEX=$END_INDEX

OUTPUT_FILE_PATH=`printf "$OUTPUT_FILE_PATH_PATTERN" 9999`
echo "=== $OUTPUT_FILE_PATH === START_INDEX: $END_INDEX ~ END_INDEX: (end)"

dlt-convert -b $START_INDEX -o "$OUTPUT_FILE_PATH" "$INPUT_FILE_PATH"
