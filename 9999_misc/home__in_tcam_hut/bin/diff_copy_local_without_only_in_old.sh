#!/bin/bash

DIFF_RESULT_FILE="patch.diff"
RESULT_ROOT="./diff_copy_result"
RESULT_SRC_ROOT="$RESULT_ROOT/00_NEW"
RESULT_DST_ROOT="$RESULT_ROOT/01_OLD"

ARG_SRC_DIR="/tmp/unknown_dir"
ARG_DST_DIR="/tmp/unknown_dir"
ARG_EXCLUDE_FILE="/tmp/unknown_file"

DEF_EXCLUDE_FILE="./def_exclude_file.lst"

function usage()
{
    echo "Usage:"
    echo "    `basename $0` <new_dir> <old_dir> [<exclude_file>]"
    echo "        <exclude_file>: "
    echo "            -: for default exclude file."
    echo ""
}

function create_def_exclude_file()
{
    cat >"$DEF_EXCLUDE_FILE" <<EOF
.git
.svn
.d
generated
gen
*.generated
*.o
*.a
*.lib
*.so
*.log
EOF

# *.obj
# *.lib
# *.exe
# *.wrn
# *.log
# *.err
# release
# retail
# debug
# obj
}

if [ $# -lt 2 ]; then
    usage
    exit 0
fi

ARG_SRC_DIR="$1"
ARG_DST_DIR="$2"

if [ $# -ge 3 ]; then
    ARG_EXCLUDE_FILE="$3"
else
    ARG_EXCLUDE_FILE=""
fi

if [ -d "$RESULT_ROOT" ]; then
    rm -fr "$RESULT_ROOT"
fi

if [ ! -d "$ARG_SRC_DIR" ]; then
    echo "*** ERROR: the source directory $ARG_SRC_DIR does not exist!" >&2
    exit 1
fi

if [ ! -d "$ARG_DST_DIR" ]; then
    echo "*** ERROR: the destination directory $ARG_DST_DIR does not exist!" >&2
    exit 1
fi

if [ "$ARG_EXCLUDE_FILE" == "-" ]; then
    create_def_exclude_file
    ARG_EXCLUDE_FILE="$DEF_EXCLUDE_FILE"
fi

if [ "$ARG_EXCLUDE_FILE" != "" -a ! -f "$ARG_EXCLUDE_FILE" ]; then
    echo "*** ERROR: the excludeing file $ARG_EXCLUDE_FILE does not exist!" >&2
    exit 1
fi

if [ -e SRCSRCSRC ]; then
    unlink SRCSRCSRC >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "*** ERROR: your must delete SRCSRCSRC manually first!" >&2
        exit 1
    fi
fi

if [ -e DSTDSTDST ]; then
    unlink DSTDSTDST >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "*** ERROR: your must delete DSTDSTDST manually first!" >&2
        exit 1
    fi
fi

rm -fr "$RESULT_SRC_ROOT"
rm -fr "$RESULT_DST_ROOT"

if [ -d "$RESULT_SRC_ROOT" -o -f "$RESULT_SRC_ROOT" -o -d "$RESULT_DST_ROOT" -o -f "$RESULT_DST_ROOT" ]; then
    echo "*** ERROR: your must delete all of the content in $RESULT_ROOT manually first!" >&2
    exit 1
fi

echo "ln -s \"$ARG_SRC_DIR\" SRCSRCSRC"
ln -s "$ARG_SRC_DIR" SRCSRCSRC
echo "ln -s \"$ARG_DST_DIR\" DSTDSTDST"
ln -s "$ARG_DST_DIR" DSTDSTDST

echo "diff -q -r SRCSRCSRC DSTDSTDST -X \"$ARG_EXCLUDE_FILE\" > \"$DIFF_RESULT_FILE\""
if [ "$ARG_EXCLUDE_FILE" == "" ]; then
    diff -q -r SRCSRCSRC DSTDSTDST > "$DIFF_RESULT_FILE"
else
    diff -q -r SRCSRCSRC DSTDSTDST -X "$ARG_EXCLUDE_FILE" > "$DIFF_RESULT_FILE"
fi

LINES=$(cat "$DIFF_RESULT_FILE" | sed -e 's/\r$//g' | sed -e 's/ /^/g' | sed -e 's/\t/^/g' | sed -e 's/\\/\\\\/g' | sed -e 's/^$/^/g')
for LINE in $LINES; do
    SRC_PATH=""
    DST_PATH=""
    SRC_PATH_DIR=""
    SRC_PATH_FILE=""
    DST_PATH_DIR=""
    DST_PATH_FILE=""

    echo "$LINE" | grep -e "\^differ$" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        SRC_PATH=$(echo "$LINE" | sed 's/^Files\^SRCSRCSRC\/\([^ \t]\+\)\^and\^DSTDSTDST\/\([^ \t]\+\)\^differ$/\1/g' | sed -e 's/\^/ /g')
        #echo "SRC: $SRC_PATH"
        SRC_PATH_DIR=$(dirname "$SRC_PATH")
        SRC_PATH_FILE=$(basename "$SRC_PATH")
        # echo "SRC_PATH_DIR: $SRC_PATH_DIR"
        # echo "SRC_PATH_FILE: $SRC_PATH_FILE"
        # echo ""
        
        
        DST_PATH=$(echo "$LINE" | sed 's/^Files\^SRCSRCSRC\/\([^ \t]\+\)\^and\^DSTDSTDST\/\([^ \t]\+\)\^differ$/\2/g' | sed -e 's/\^/ /g')
        #echo "DST: $DST_PATH"
        DST_PATH_DIR=$(dirname "$DST_PATH")
        DST_PATH_FILE=$(basename "$DST_PATH")
        # echo "DST_PATH_DIR: $DST_PATH_DIR"
        # echo "DST_PATH_FILE: $DST_PATH_FILE"
        # echo ""
    fi
    
    echo "$LINE" | grep -e "^Only\^in\^SRCSRCSRC/" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        SRC_PATH_DIR=$(echo "$LINE" | sed 's/^Only\^in^SRCSRCSRC\/\([^:]\+\):\^\([^ ]\+\)$/\1/g' | sed -e 's/\^/ /g')
        SRC_PATH_FILE=$(echo "$LINE" | sed 's/^Only\^in^SRCSRCSRC\/\([^:]\+\):\^\([^ ]\+\)$/\2/g')
        #echo "SRC: $SRC_PATH_DIR/$SRC_PATH_FILE"
        # echo "SRC_PATH_DIR: $SRC_PATH_DIR"
        # echo "SRC_PATH_FILE: $SRC_PATH_FILE"
        # echo ""
    fi
    
    echo "$LINE" | grep -e "^Only\^in\^SRCSRCSRC:^" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        SRC_PATH_DIR=.
        SRC_PATH_FILE=$(echo "$LINE" | sed 's/^Only\^in^SRCSRCSRC:\^\([^ ]\+\)$/\1/g')
        #echo "SRC: $SRC_PATH_DIR/$SRC_PATH_FILE"
        # echo "SRC_PATH_DIR: $SRC_PATH_DIR"
        # echo "SRC_PATH_FILE: $SRC_PATH_FILE"
        # echo ""
    fi
    
    echo "$LINE" | grep -e "^Only\^in\^DSTDSTDST/" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        DST_PATH_DIR=$(echo "$LINE" | sed 's/^Only\^in^DSTDSTDST\/\([^:]\+\):\^\([^ ]\+\)$/\1/g' | sed -e 's/\^/ /g')
        DST_PATH_FILE=$(echo "$LINE" | sed 's/^Only\^in^DSTDSTDST\/\([^:]\+\):\^\([^ ]\+\)$/\2/g')
        #echo "DST: $DST_PATH_DIR/$DST_PATH_FILE"
        # echo "DST_PATH_DIR: $DST_PATH_DIR"
        # echo "DST_PATH_FILE: $DST_PATH_FILE"
        # echo ""
        continue
    fi
    
    echo "$LINE" | grep -e "^Only\^in\^DSTDSTDST:^" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        DST_PATH_DIR=.
        DST_PATH_FILE=$(echo "$LINE" | sed 's/^Only\^in^DSTDSTDST:\^\([^ ]\+\)$/\1/g')
        #echo "DST: $DST_PATH_DIR/$DST_PATH_FILE"
        # echo "DST_PATH_DIR: $DST_PATH_DIR"
        # echo "DST_PATH_FILE: $DST_PATH_FILE"
        # echo ""
        continue
    fi
    
    if [ "$SRC_PATH_FILE" != "" ]; then
        mkdir -p "$RESULT_SRC_ROOT/$SRC_PATH_DIR"
        echo "cp -pfr \"SRCSRCSRC/$SRC_PATH_DIR/$SRC_PATH_FILE\" \"$RESULT_SRC_ROOT/$SRC_PATH_DIR\""
        cp -pfr "SRCSRCSRC/$SRC_PATH_DIR/$SRC_PATH_FILE" "$RESULT_SRC_ROOT/$SRC_PATH_DIR/$SRC_PATH_FILE"
    fi
    
    if [ "$DST_PATH_FILE" != "" ]; then
        mkdir -p "$RESULT_DST_ROOT/$DST_PATH_DIR"
        echo "cp -pfr \"DSTDSTDST/$DST_PATH_DIR/$DST_PATH_FILE\" \"$RESULT_DST_ROOT/$DST_PATH_DIR\""
        cp -pfr "DSTDSTDST/$DST_PATH_DIR/$DST_PATH_FILE" "$RESULT_DST_ROOT/$DST_PATH_DIR/$DST_PATH_FILE"
    fi
done

mv "$RESULT_SRC_ROOT" "$RESULT_ROOT/00_new"
mv "$RESULT_DST_ROOT" "$RESULT_ROOT/01_old"

