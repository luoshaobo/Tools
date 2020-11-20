#!/bin/bash

PERSISTENCE_FILE="tp-pers-install.tar.gz"

if [ $# -ge 1 ]; then
    PERSISTENCE_FILE="$1"
fi

if [ ! -r "$PERSISTENCE_FILE" ]; then
    echo "*** ERROR: the file can't be read: $PERSISTENCE_FILE" >&1
    exit 1
fi

# 7.2
# 7.2.1
echo "adb kill-server"
adb kill-server
echo "sleep 3"
sleep 3
echo "adb devices"
adb devices

echo "sleep 5"
sleep 5

# 7.5
echo "adb wait-for-device"
adb wait-for-device
echo "adb shell rm -rf /data/persistency"
adb shell rm -rf /data/persistency
echo "sleep 3"
sleep 3
echo "adb push $PERSISTENCE_FILE /tmp/tp-pers-install.tar.gz"
adb push $PERSISTENCE_FILE /tmp/tp-pers-install.tar.gz
echo "sleep 3"
sleep 3
echo "adb shell pers-resource-installer /tmp/tp-pers-install.tar.gz"
adb shell pers-resource-installer /tmp/tp-pers-install.tar.gz
echo "sleep 3"
sleep 3
echo "adb shell persistent_key_writer factory 0xFFFFFFFE CONSOLE-ENABLE integer 1"
adb shell persistent_key_writer factory 0xFFFFFFFE CONSOLE-ENABLE integer 1
echo "sleep 3"
sleep 3
echo "adb shell sync"
adb shell sync
echo "sleep 3"
sleep 3
echo "adb shell sync"
adb shell sync
echo "sleep 3"
sleep 3


#echo "adb shell sys_reboot"
#adb shell sys_reboot
