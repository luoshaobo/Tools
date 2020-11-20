#!/bin/bash

INPUT_LOG_FILE="$1"
OUTPUT_CPU_ETC="${INPUT_LOG_FILE}__CPU__CPU_etc.txt"

OUTPUT_IDLE="${INPUT_LOG_FILE}__CPU__IDEL.txt"
OUTPUT_IDLE__STATISTICS="${INPUT_LOG_FILE}__CPU__IDEL__statistics.txt"

OUTPUT_USED="${INPUT_LOG_FILE}__CPU__USED.txt"
OUTPUT_USED__STATISTICS="${INPUT_LOG_FILE}__CPU__USED__statistics.txt"

OUTPUT_USR="${INPUT_LOG_FILE}__CPU__USR.txt"
OUTPUT_USR__STATISTICS="${INPUT_LOG_FILE}__CPU__USR__statistics.txt"

OUTPUT_SYS="${INPUT_LOG_FILE}__CPU__SYS.txt"
OUTPUT_SYS__STATISTICS="${INPUT_LOG_FILE}__CPU__SYS__statistics.txt"


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
    cat "$INPUT_LOG_FILE" | grep "~TOPC~ CPU~" | grep "CPU:" > "$OUTPUT_CPU_ETC"

    cat "$INPUT_LOG_FILE" | grep "~TOPC~ CPU~" | grep "CPU:" | awk '{ print $11 }' | sed 's/%//g' > "$OUTPUT_IDLE"
    gen_statistics "$OUTPUT_IDLE" "$OUTPUT_IDLE__STATISTICS"

    ### {{{
    cat "$INPUT_LOG_FILE" | grep "~TOPC~ CPU~" | grep "CPU:" | awk '{ print 100.0 - $11 }' | sed 's/%//g' | bc -l > "$OUTPUT_USED"
    gen_statistics "$OUTPUT_USED" "$OUTPUT_USED__STATISTICS"
    ### }}}

    cat "$INPUT_LOG_FILE" | grep "~TOPC~ CPU~" | grep "CPU:" | awk '{ print $5 }' | sed 's/%//g' > "$OUTPUT_USR"
    gen_statistics "$OUTPUT_USR" "$OUTPUT_USR__STATISTICS"

    cat "$INPUT_LOG_FILE" | grep "~TOPC~ CPU~" | grep "CPU:" | awk '{ print $7 }' | sed 's/%//g' > "$OUTPUT_SYS"
    gen_statistics "$OUTPUT_SYS" "$OUTPUT_SYS__STATISTICS"
}

if [ ! -r "$INPUT_LOG_FILE" ]; then
    echo "*** ERROR: \$1 can not be read: $INPUT_LOG_FILE" >&2
    exit 1
fi

process
