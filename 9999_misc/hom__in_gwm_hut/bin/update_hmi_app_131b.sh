#!/bin/sh

NOT_RESTART="FALSE"

if [ $# -ge 1 ]; then
    if [ "$1" == "not_restart" ]; then
        NOT_RESTART="TRUE"
    fi
fi

HMI_BIN_REMOTE_PATH__BASE="/home/uidv4956/build_temp/13.18.10.01/131b/build_hmi_app"
HMI_BIN_REMOTE_PATH="$HMI_BIN_REMOTE_PATH__BASE/hmi-application/hmi-application-131b"
HMI_BIN_UIKIT_CYC_PATH="$HMI_BIN_REMOTE_PATH__BASE/deps/cleanroom-uikit/lib/libuikit_universe_CYC.so"

if [ ! -f "$HMI_BIN_REMOTE_PATH" ]; then
    echo "*** Error: file \"$HMI_BIN_REMOTE_PATH\" does not exists!" >&2
    exit 1
fi

echo "cp \"$HMI_BIN_REMOTE_PATH\" /usr/bin/ovip/HMI/hmi-application"
cp "$HMI_BIN_REMOTE_PATH" /usr/bin/ovip/HMI/hmi-application
echo "cp \"$HMI_BIN_UIKIT_CYC_PATH\" /usr/lib"
cp "$HMI_BIN_UIKIT_CYC_PATH" /usr/lib
sync

if [ "$NOT_RESTART" != "TRUE" ]; then
    systemctl --system daemon-reload
    systemctl restart ovip-sys-hmi-application
fi
