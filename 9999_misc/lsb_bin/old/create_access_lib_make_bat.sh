#!/bin/bash

ACCESS_LIB_BASE_DIR='D:\casdev\WinCE\public\MMP_PROD'
ACCESS_LIB_DIRS=`cat <<EOF
_AV\Main\AccessLib
_DICTA\AccessLib
_ELY\Backlight\AccessLib
_ELY\Main\AccessLib
_ELY\RMI\AccessLib
_ELY\SplashScreen\AccessLib
_HS\GPIOPinToggle\AccessLib
_HS\HMISWVersion\AccessLib
_HS\TouchScreen\AccessLib
_LCM\Main\AccessLib
_MED\BTMedia\AccessLib
_MED\Cluster\AccessLib
_MED\DataInfoDisp\AccessLib
_MED\Diagnostic\AccessLib
_MED\Dvd\AccessLib
_MED\iPod\AccessLib
_MED\USBAudio\AccessLib
_MED\USBPicture\AccessLib
_MED\USB_Video\AccessLib
_NAV\Main\AccessLib
_RAD\Main\AccessLib
_SYS\Alerts\AccessLib
_SYS\CommMOST\AccessLib
_SYS\Configuration\AccessLib
_SYS\EngineeringMode\AccessLib
_SYS\HWVariant\AccessLib
_SYS\Lifecycle\AccessLib
_SYS\SWLoading\AccessLib
_SYS\SysApplication\AccessLib
_SYS\SysClimate\AccessLib
_SYS\SysCluster\AccessLib
_TELE\AccessLib
EOF
`

echo "@echo off"
echo ""

for ACCESS_LIB_DIR in $ACCESS_LIB_DIRS; do
    echo "echo ####################################################################################"
    echo "echo ######## {{{ $ACCESS_LIB_DIR"
    echo "cd $ACCESS_LIB_BASE_DIR\\$ACCESS_LIB_DIR"
    echo "build -c"
    echo "echo ######## $ACCESS_LIB_DIR }}}"
    echo ""
done
