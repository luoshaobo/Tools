#!/bin/bash

CONST_ERRNO_ARGS_TOO_LESS=1
CONST_ERRNO_FILE_CANT_BE_READ=2
CONST_ERRNO_BEGINNING_LINE_NOT_FOUNT=3
CONST_ERRNO_TOO_MANY_BEGINNING_LINES=4

DEBUG="${DEBUG:-0}"

TARGET_FILE_PATH="$1"
BEGINNING_LINE_PARTIAL_CONTENT="$2"
SCOPE_LINE_COUNT="$3"

TMP_FILE=""

function print_info_msg()
{
    local MSG

    MSG="$1"

    if [ $DEBUG -eq 1 ]; then
        echo "$MSG" >&2
    fi
}

function print_error_msg()
{
    local MSG

    MSG="$1"

    echo "$MSG" >&2
}

function usage()
{
    echo "usage:" >&2
    echo "    `basename $0` <target_file_path> <beginning_line_partial_content> <line_count> <args_to_sed ...>" >&2
}

function main()
{
    local BEGINNING_LINE_NO
    local N
    local END_LINE_NO
    local SED_CMD

    print_info_msg "TARGET_FILE_PATH: $TARGET_FILE_PATH"
    print_info_msg "BEGINNING_LINE_PARTIAL_CONTENT: $BEGINNING_LINE_PARTIAL_CONTENT"
    print_info_msg "SCOPE_LINE_COUNT: $SCOPE_LINE_COUNT"

    if [ $# -lt 4 ]; then
        usage
        return $CONST_ERRNO_ARGS_TOO_LESS
    fi

    if [ "$TARGET_FILE_PATH" = "-" ]; then
        TMP_FILE=`mktemp`
        TARGET_FILE_PATH="$TMP_FILE"
        cat /dev/stdin > "$TARGET_FILE_PATH"
    fi

    if [ ! -r "$TARGET_FILE_PATH" ]; then
        print_error_msg "*** ERROR: file can't be read: $TARGET_FILE_PATH"
        return $CONST_ERRNO_FILE_CANT_BE_READ
    fi

    BEGINNING_LINE_NO=`grep -n "$BEGINNING_LINE_PARTIAL_CONTENT" "$TARGET_FILE_PATH" | awk -F: '{ print $1 }'`
    if [ -z "$BEGINNING_LINE_NO" ]; then
        print_error_msg "*** ERROR: no line found with BEGINNING_LINE_PARTIAL_CONTENT: $BEGINNING_LINE_PARTIAL_CONTENT"
        return $CONST_ERRNO_BEGINNING_LINE_NOT_FOUNT
    fi
    print_info_msg "BEGINNING_LINE_NO: $BEGINNING_LINE_NO"

    N=`echo "$BEGINNING_LINE_NO" | wc -l`
    print_info_msg "N: $N"
    if [ "$N" -gt 1 ]; then
        print_error_msg "*** ERROR: too many lines found with BEGINNING_LINE_PARTIAL_CONTENT: $BEGINNING_LINE_PARTIAL_CONTENT"
        return $CONST_ERRNO_TOO_MANY_BEGINNING_LINES
    fi

    END_LINE_NO=$(($BEGINNING_LINE_NO + $SCOPE_LINE_COUNT - 1))
    print_info_msg "END_LINE_NO: $END_LINE_NO"

    shift 3

    SED_CMD="sed -e '$BEGINNING_LINE_NO,$END_LINE_NO$*' $TARGET_FILE_PATH"
    print_info_msg "SED_CMD: $SED_CMD"
    eval "$SED_CMD"
}

main $*
ERROR_CODE=$?
if [ ! -z "$TMP_FILE" ]; then
    rm -f "$TMP_FILE"
fi
exit $ERROR_CODE
