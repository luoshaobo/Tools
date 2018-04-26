#!/bin/bash

REMOTE_IP="192.168.1.3"
REMOTE_USER="anonymous"
REMOTE_PWD="x@y"

REMOTE_BASE_DIR="/NAND_RES/entHMI/resources"
LOCAL_BASE_DIR="."                                      # D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Tables

function mkdir_on_remote()
{
    local DIR_TO_CREATE
    
    DIR_TO_CREATE="$1"
    if [ -z "$DIR_TO_CREATE" ]; then
        echo "*** ERROR: mkdir_on_remote(): \$DIR_TO_CREATE is empty!" >&2
        exit 1
    fi
    
    ftp -n -v $REMOTE_IP <<EOF
user $REMOTE_USER $REMOTE_PWD
binary
hash
prompt
cd "$REMOTE_BASE_DIR"
mkdir "$DIR_TO_CREATE"
by
EOF
}

function put_files_to_remote()
{
    local DIR_WITH_FILES
    
    DIR_WITH_FILES="$1"
    if [ -z "$DIR_WITH_FILES" ]; then
        echo "*** ERROR: put_files_to_remote(): \$DIR_WITH_FILES is empty!" >&2
        exit 1
    fi
    
    ftp -n -v $REMOTE_IP <<EOF
user $REMOTE_USER $REMOTE_PWD
binary
hash
prompt
cd "$REMOTE_BASE_DIR/$DIR_WITH_FILES"
lcd "$LOCAL_BASE_DIR/$DIR_WITH_FILES"
mput *
by
EOF
}

function put_file_to_remote()
{
    local FILE_PATH
    
    FILE_PATH="$1"
    if [ -z "$FILE_PATH" ]; then
        echo "*** ERROR: put_file_to_remote(): \$FILE_PATH is empty!" >&2
        exit 1
    fi
    
    ftp -n -v $REMOTE_IP <<EOF
user $REMOTE_USER $REMOTE_PWD
binary
hash
prompt
cd "$REMOTE_BASE_DIR"
lcd "$LOCAL_BASE_DIR"
put "$FILE_PATH" "$FILE_PATH"
by
EOF
}

echo "*** Do you want to update all files in local dir \"`(cd $LOCAL_BASE_DIR && pwd)`\" to remote dir \"$REMOTE_BASE_DIR\"?" >&2
read ANSWER
if [ "$ANSWER" = 'y' -o "$ANSWER" = 'Y' -o "$ANSWER" = 'yes' -o "$ANSWER" = 'YES' ]; then
    :
else
    exit 0
fi

DIRS=`find $LOCAL_BASE_DIR -type d | sed 's/ /^/'`
for DIR in $DIRS; do
    DIR_ORIG=`echo "$DIR" | sed 's/\^/ /'`
    if [ "$DIR_ORIG" = "." -o "$DIR_ORIG" = ".." ]; then
        continue
    fi
    
    if [ $# -le 2 -a "$1" = "mkdir" ]; then
        mkdir_on_remote "$DIR_ORIG"
    else
        put_files_to_remote "$DIR_ORIG"
    fi
done

























