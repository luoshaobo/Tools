#!/bin/bash

DLT_RECEIVE_FILTER_FILE_PATH=`mktemp "/tmp/dlt_receive_filter.XXXXXXXXXX"`
DLT_DEAMON_HOST="$1"

function help
{
    echo "Usage:"
    echo "  `basename $0` <dlt_deamon_host> [<arg1_to_forward> [<arg2_to_forward> ...]]"
}

function main
{
    if [ $# -lt 1 ]; then
        help
        exit 1
    fi

    echo "TOPC CPU" > "$DLT_RECEIVE_FILTER_FILE_PATH"

    shift 1
    dlt-receive -f "$DLT_RECEIVE_FILTER_FILE_PATH" -a "$DLT_DEAMON_HOST" \
        | check_cpu_usage_overflow.py \
            --input-cpu-summary-file-path="stdin" \
            --output-result-file-path="stdout" \
            --cpu-usage-overflow-max-limit=90 \
            --cpu-usage-overflow-min-time=30 \
            $*

    rm -f "$DLT_RECEIVE_FILTER_FILE_PATH"
}

main $*
