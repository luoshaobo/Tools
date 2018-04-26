#!/bin/bash

REMOTE_IP="192.168.1.3"
REMOTE_USER="anonymous"
REMOTE_PWD="x@y"

REMOTE_BASE_DIR="/"
LOCAL_BASE_DIR="."

LOG_CONFIG_FILE_NAME="ssw_LtConfiguration.xml"

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

function execute_cmd_on_remote()
{
    local CMD
    
    CMD="$1"
    if [ -z "$CMD" ]; then
        echo "*** ERROR: execute_cmd_on_remote(): \$CMD is empty!" >&2
        exit 1
    fi
    
    expect <<EOF
spawn telnet "$REMOTE_IP"
expect { 
    "\> " { 
        send " $CMD\r" 
    }
}
expect { 
    "\> " { 
        send " exit\r" 
    }
}
expect eof {
    send_user "\r\n"
    exit 0
}
EOF
}

REMOTE_BASE_DIR="/Documents and Settings"
LOCAL_BASE_DIR=$(cd "D:\lsb\work_dir\DPCA\my_doc\doc_important\14_log_configure\my_config" && pwd)
if [ ! -d "$LOCAL_BASE_DIR" ]; then
    echo "*** ERROR: the dir \"$LOCAL_BASE_DIR\" doesn't exits!" >&2
    exit 1
fi

if [ ! -f "$LOCAL_BASE_DIR/$LOG_CONFIG_FILE_NAME" ]; then
    echo "*** ERROR: the file \"$LOCAL_BASE_DIR/$LOG_CONFIG_FILE_NAME\" doesn't exits!" >&2
    exit 1
fi
put_file_to_remote "$LOG_CONFIG_FILE_NAME"

REMOTE_BASE_DIR="/Windows"
LOCAL_BASE_DIR=$(cd "D:\casdev\WinCE\release\abase_a4low2.mmp_prod.imx53_ard.retail" && pwd)
put_file_to_remote "ssw_ltconfigconvert.exe"
put_file_to_remote "iltool.exe"

execute_cmd_on_remote "ssw_ltconfigconvert.exe"
execute_cmd_on_remote "iltool.exe -f"
execute_cmd_on_remote "iltool.exe -f"


















