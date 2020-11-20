#!/bin/bash

export TOOLKIT_PKG_PATHNAME="D:\my_bin\tcam\toolkit_on_target_daily_latest.tgz"
if [ $# -ge 1 ]; then
    TOOLKIT_PKG_PATHNAME="$1"
fi
echo "+++ The toolkit package is: ${TOOLKIT_PKG_PATHNAME}"

if [ ! -f "${TOOLKIT_PKG_PATHNAME}" ]; then
    echo "*** ERROR: The toolkit package file not exists: ${TOOLKIT_PKG_PATHNAME}"
    exit 1
fi

TOOLKIT_PKG_NAME=`basename "$TOOLKIT_PKG_PATHNAME"`

# install tk
echo "adb push \"${TOOLKIT_PKG_PATHNAME}\" /data"
adb push "${TOOLKIT_PKG_PATHNAME}" /data
echo "sleep 6"
sleep 6
echo "adb shell sync"
adb shell sync

echo "adb shell \"cd /data && tar zxf ${TOOLKIT_PKG_NAME}\""
adb shell "cd /data && tar zxf ${TOOLKIT_PKG_NAME}"
echo "sleep 6"
sleep 6
echo "adb shell sync"
adb shell sync

echo "adb shell \"cd /data/toolkit_on_target && sh ./install.sh\""
adb shell "cd /data/toolkit_on_target && sh ./install.sh"
echo "sleep 6"
sleep 6
echo "adb shell sync"
adb shell sync

echo "adb shell sys_reboot"
adb shell sys_reboot

