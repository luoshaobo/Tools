#!/bin/bash

# 7.2
# 7.2.1
echo "adb kill-server"
adb kill-server
echo "sleep 3"
sleep 3
echo "adb devices"
adb devices
echo "adb shell sys_reboot bootloader"
adb shell sys_reboot bootloader
echo "adb devices"
adb devices
echo "fastboot devices"
fastboot devices

echo "sleep 10"
sleep 10

# 7.2.2
echo "fastboot flash ubi ubi.img"
fastboot flash ubi ubi.img
echo "fastboot flash aboot appsboot.mbn"
fastboot flash aboot appsboot.mbn
echo "fastboot flash boot boot.img "
fastboot flash boot boot.img 
echo "fastboot reboot"
fastboot reboot

echo "sleep 20"
sleep 20

# 7.3
echo "adb kill-server"
adb kill-server
echo "sleep 6"
sleep 6
echo "adb devices"
adb devices
echo "adb shell sys_reboot bootloader"
adb shell sys_reboot bootloader
echo "fastboot flash test test.ubifs"
fastboot flash test test.ubifs
echo "fastboot reboot"
fastboot reboot

echo "sleep 20"
sleep 20

# 7.4
echo "adb kill-server"
adb kill-server
echo "sleep 6"
sleep 6
echo "adb devices"
adb devices
echo "adb push update_compatibility.db /data/tcam"
adb push update_compatibility.db /data/tcam

echo "sleep 3"
sleep 3

# 7.5
echo "adb wait-for-device"
adb wait-for-device
echo "adb shell rm -rf /data/persistency"
adb shell rm -rf /data/persistency
echo "sleep 3"
sleep 3
echo "adb push tp-pers-install.tar.gz /tmp/tp-pers-install.tar.gz"
adb push tp-pers-install.tar.gz /tmp/tp-pers-install.tar.gz
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

##########################################################################################
## ttcam system swappl
##

echo "adb shell ttcam system swappl"
adb shell ttcam system swappl
echo "sleep 30"
sleep 30

echo "adb kill-server"
adb kill-server
echo "sleep 6"
sleep 6
echo "adb devices"
adb devices

##########################################################################################
## change persistence keys
##

echo "adb shell persistent_key_writer root 0xFFFFFFFE tls_flag integer 0"
adb shell persistent_key_writer root 0xFFFFFFFE tls_flag integer 0

echo "adb shell persistent_key_writer root 0xFFFFFFFE message-delivery_MQTT_SERVER_URL string \"198.18.32.18:1883\""
adb shell persistent_key_writer root 0xFFFFFFFE message-delivery_MQTT_SERVER_URL string "198.18.32.18:1883"

echo "adb shell sync"
adb shell sync

##########################################################################################
## remove something to keep some space in rootfs
##
echo "sleep 6"
sleep 6

echo "adb shell  \"mount -o remount,rw /\""
adb shell  "mount -o remount,rw /"

echo "adb shell  \"rm /usr/lib/*.a\""
adb shell  "rm /usr/lib/*.a"

echo "sleep 6"
sleep 6

##########################################################################################
## flash toolkit
##

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

##########################################################################################
## other changes
##
echo "adb shell mv /etc/DefaultBigDataServiceConfigs.xml /etc/DefaultBigDataServiceConfigs.xml.orig"
adb shell mv /etc/DefaultBigDataServiceConfigs.xml /etc/DefaultBigDataServiceConfigs.xml.orig

echo "adb shell mv /etc/static_arp_bind_flag_file /etc/static_arp_bind_flag_file.orig"
adb shell mv /etc/static_arp_bind_flag_file /etc/static_arp_bind_flag_file.orig

export DLT_SO="D:\my_bin\tcam\libdlt.so.2.17.0"
echo "adb push \"${DLT_SO}\" /data"
adb push "${DLT_SO}" /data

echo "sleep 6"
sleep 6
echo "adb shell cp -f /data/libdlt.so.2.17.0 /usr/lib/"
adb shell cp -f /data/libdlt.so.2.17.0 /usr/lib/

echo "adb shell sync"
adb shell sync

##########################################################################################
## reboot
##

echo "adb shell sys_reboot"
adb shell sys_reboot

