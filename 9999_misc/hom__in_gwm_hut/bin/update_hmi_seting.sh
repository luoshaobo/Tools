#!/bin/sh

NOT_RESTART="FALSE"

if [ $# -ge 1 ]; then
    if [ "$1" == "not_restart" ]; then
        NOT_RESTART="TRUE"
    fi
fi

HMI_BIN_REMOTE_PATH="/home/uidv4956/build_temp/00.17.34.01/build_hmi_setting/src/HMISettingApp/hmi-setting-app"

if [ ! -f "$HMI_BIN_REMOTE_PATH" ]; then
    echo "*** Error: file \"$HMI_BIN_REMOTE_PATH\" does not exists!" >&2
    exit 1
fi

cp "$HMI_BIN_REMOTE_PATH" /usr/bin/ovip/HMI/
sync

if [ "$NOT_RESTART" != "TRUE" ]; then
    systemctl restart ovip-sys-hmi-settings
fi
