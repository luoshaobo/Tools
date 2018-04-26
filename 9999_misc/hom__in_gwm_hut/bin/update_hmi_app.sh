#!/bin/sh

NOT_RESTART="FALSE"

if [ $# -ge 1 ]; then
    if [ "$1" == "not_restart" ]; then
        NOT_RESTART="TRUE"
    fi
fi

HMI_BIN_REMOTE_PATH__BASE="/home/uidv4956/build_temp/13.18.10.01/131/build_hmi_app"
HMI_BIN_REMOTE_PATH="$HMI_BIN_REMOTE_PATH__BASE/hmi-application/hmi-application"
HMI_BIN_UIKIT_DYC_PATH="$HMI_BIN_REMOTE_PATH__BASE/deps/cleanroom-uikit/lib/libuikit_universe_DYC.so"
HMI_BIN_UIKIT_AOC_PATH="$HMI_BIN_REMOTE_PATH__BASE/deps/cleanroom-uikit/lib/libuikit_universe_AOC.so"

if [ ! -f "$HMI_BIN_REMOTE_PATH" ]; then
    echo "*** Error: file \"$HMI_BIN_REMOTE_PATH\" does not exists!" >&2
    exit 1
fi

echo "cp \"$HMI_BIN_REMOTE_PATH\" /usr/bin/ovip/HMI/hmi-application"
cp "$HMI_BIN_REMOTE_PATH" /usr/bin/ovip/HMI/hmi-application
echo "cp \"$HMI_BIN_UIKIT_DYC_PATH\" /usr/lib"
cp "$HMI_BIN_UIKIT_DYC_PATH" /usr/lib
echo "cp \"$HMI_BIN_UIKIT_AOC_PATH\" /usr/lib"
cp "$HMI_BIN_UIKIT_AOC_PATH" /usr/lib
sync

if [ "$NOT_RESTART" != "TRUE" ]; then
    systemctl --system daemon-reload
    systemctl restart ovip-sys-hmi-application
fi
