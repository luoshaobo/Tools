#!/bin/bash

INPUT_LOG_FILE="$1"
OUTPUT_MEM_TOTAL_ETC="${INPUT_LOG_FILE}__RAM__MemTotal_etc.txt"
OUTPUT_SLAB_ETC="${INPUT_LOG_FILE}__RAM__slab_etc.txt"

OUTPUT_ANON="${INPUT_LOG_FILE}__RAM__ANON.txt"
OUTPUT_ANON__STATISTICS="${INPUT_LOG_FILE}__RAM__ANON__statistics.txt"

OUTPUT_MAP="${INPUT_LOG_FILE}__RAM__MAP.txt"
OUTPUT_MAP__STATISTICS="${INPUT_LOG_FILE}__RAM__MAP__statistics.txt"

OUTPUT_FREE="${INPUT_LOG_FILE}__RAM__FREE.txt"
OUTPUT_FREE__STATISTICS="${INPUT_LOG_FILE}__RAM__FREE__statistics.txt"


OUTPUT_SLAB="${INPUT_LOG_FILE}__RAM__SLAB.txt"
OUTPUT_SLAB__STATISTICS="${INPUT_LOG_FILE}__RAM__SLAB__statistics.txt"

OUTPUT_CACHE="${INPUT_LOG_FILE}__RAM__CACHE.txt"
OUTPUT_CACHE__STATISTICS="${INPUT_LOG_FILE}__RAM__CACHE__statistics.txt"

OUTPUT_DIRTY="${INPUT_LOG_FILE}__RAM__DIRTY.txt"
OUTPUT_DIRTY__STATISTICS="${INPUT_LOG_FILE}__RAM__DIRTY__statistics.txt"

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
    cat "$INPUT_LOG_FILE" | grep "~TOPM~ MEM~" | grep "Mem total:" > "$OUTPUT_MEM_TOTAL_ETC"
    cat "$INPUT_LOG_FILE" | grep "~TOPM~ MEM~" | grep "slab:" > "$OUTPUT_SLAB_ETC"

    cat "$OUTPUT_MEM_TOTAL_ETC" | grep "~TOPM~ MEM~" | grep "Mem total:" | awk '{ print $6 }' | sed 's/anon://g' > "$OUTPUT_ANON"
    gen_statistics "$OUTPUT_ANON" "$OUTPUT_ANON__STATISTICS"

    cat "$OUTPUT_MEM_TOTAL_ETC" | grep "~TOPM~ MEM~" | grep "Mem total:" | awk '{ print $7 }' | sed 's/map://g' > "$OUTPUT_MAP"
    gen_statistics "$OUTPUT_MAP" "$OUTPUT_MAP__STATISTICS"

    cat "$OUTPUT_MEM_TOTAL_ETC" | grep "~TOPM~ MEM~" | grep "Mem total:" | awk '{ print $8 }' | sed 's/free://g' > "$OUTPUT_FREE"
    gen_statistics "$OUTPUT_FREE" "$OUTPUT_FREE__STATISTICS"


    cat "$OUTPUT_SLAB_ETC" | grep "~TOPM~ MEM~" | grep "slab:" | awk '{ print $5 }' | sed 's/slab://g' > "$OUTPUT_SLAB"
    gen_statistics "$OUTPUT_SLAB" "$OUTPUT_SLAB__STATISTICS"

    cat "$OUTPUT_SLAB_ETC" | grep "~TOPM~ MEM~" | grep "slab:" | awk '{ print $7 }' | sed 's/cache://g' > "$OUTPUT_CACHE"
    gen_statistics "$OUTPUT_CACHE" "$OUTPUT_CACHE__STATISTICS"

    cat "$OUTPUT_SLAB_ETC" | grep "~TOPM~ MEM~" | grep "slab:" | awk '{ print $8 }' | sed 's/dirty://g' > "$OUTPUT_DIRTY"
    gen_statistics "$OUTPUT_DIRTY" "$OUTPUT_DIRTY__STATISTICS"
}

if [ ! -r "$INPUT_LOG_FILE" ]; then
    echo "*** ERROR: \$1 can not be read: $INPUT_LOG_FILE" >&2
    exit 1
fi

process
