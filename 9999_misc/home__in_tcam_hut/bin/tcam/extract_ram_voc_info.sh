#!/bin/bash

APP_BIN_PATH="/usr/bin/voc"

INPUT_LOG_FILE="$1"
OUTPUT_MEM_VOC_ALL="${INPUT_LOG_FILE}__RAM__voc_all.txt"

OUTPUT_RSS="${INPUT_LOG_FILE}__RAM__voc_RSS.txt"
OUTPUT_RSS_STATISTICS="${INPUT_LOG_FILE}__RAM__voc_RSS__statistics.txt"

function gen_statistics
{
    local INPUT_FILE
    local OUTPUT_FILE

    INPUT_FILE="$1"
    OUTPUT_FILE="$2"

    > "$OUTPUT_FILE"
    cat "$INPUT_FILE" | awk -N -M '{sum+=$1} END {print "Average =", sum/NR}' >> "$OUTPUT_FILE"

    echo -n "Max = " >> "$OUTPUT_FILE"
    cat "$INPUT_FILE" | sort -n | tail -n 1 >> "$OUTPUT_FILE"

    echo -n "Min = " >> "$OUTPUT_FILE"
    cat "$INPUT_FILE" | sort -n | head -n 1 >> "$OUTPUT_FILE"
}

function process
{
    cat "$INPUT_LOG_FILE" | grep "~TOPM~ MEM~" | grep "$APP_BIN_PATH" > "$OUTPUT_MEM_VOC_ALL"

    cat "$OUTPUT_MEM_VOC_ALL" | awk '{ print $8 }' > "$OUTPUT_RSS"
    gen_statistics "$OUTPUT_RSS" "$OUTPUT_RSS_STATISTICS"
}

if [ ! -r "$INPUT_LOG_FILE" ]; then
    echo "*** ERROR: \$1 can not be read: $INPUT_LOG_FILE" >&2
    exit 1
fi

process
