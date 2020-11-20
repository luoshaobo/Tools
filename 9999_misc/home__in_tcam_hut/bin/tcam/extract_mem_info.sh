#!/bin/bash

INPUT_LOG_FILE="$1"
OUTPUT_MEM_ALL="${INPUT_LOG_FILE}__MEM__all.txt"

OUTPUT_MEM_AVAILABLE="${INPUT_LOG_FILE}__MEM__MemAvailable.txt"
OUTPUT_MEM_AVAILABLE__STATISTICS="${INPUT_LOG_FILE}__MEM__MemAvailable__statistics.txt"

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
    cat "$INPUT_LOG_FILE" | grep "~MEMI~MEMI~" > "$OUTPUT_MEM_ALL"

    cat "$OUTPUT_MEM_ALL" | grep "MemAvailable:"| awk '{ print $3 }' > "$OUTPUT_MEM_AVAILABLE"
    gen_statistics "$OUTPUT_MEM_AVAILABLE" "$OUTPUT_MEM_AVAILABLE__STATISTICS"

}

if [ ! -r "$INPUT_LOG_FILE" ]; then
    echo "*** ERROR: \$1 can not be read: $INPUT_LOG_FILE" >&2
    exit 1
fi

process
