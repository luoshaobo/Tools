#!/bin/bash

ADK_INSTALLER_ROOT_DIR="$1"
WORK_DIR="$2"
VERSION="$3"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <ADK_INSTALLER_ROOT_DIR> <WORK_DIR> <VERSION>"
    echo "       <ADK_INSTALLER_ROOT_DIR>: it may be \"default\""
    echo "       <VSERSION>: like \"13.11.03.02\""
    echo ""
}

if [ $# -lt 3 ]; then
    usage
    exit
fi

#echo "ADK_INSTALLER_ROOT_DIR=$ADK_INSTALLER_ROOT_DIR"

if [ "$ADK_INSTALLER_ROOT_DIR" = "default" ]; then
    HMI_IPK_FILE_PATH=`ls /PROJ/oip/GWM_MY18/MY18_SyDK/SyDK_${VERSION}_*/Mirror/solutions/com.continental/MV_SYSINT/prebuilt/arm-mv-linux/staging-sys-hmi-armv7a-mv-linux_${VERSION}-r0_arm-mv-linux_*.ipk`
else
    HMI_IPK_FILE_PATH=`ls $ADK_INSTALLER_ROOT_DIR/Mirror/solutions/com.continental/MV_SYSINT/prebuilt/arm-mv-linux/staging-sys-hmi-armv7a-mv-linux_${VERSION}-r0_arm-mv-linux_*.ipk`
fi
    
echo "HMI_IPK_FILE_PATH=$HMI_IPK_FILE_PATH"
    
WORK_TMP_DIR="$WORK_DIR/tmp"

if [ ! -f "$HMI_IPK_FILE_PATH" ]; then
    echo "*** Error: wrong HMI_IPK_FILE_PATH: $HMI_IPK_FILE_PATH" >&2
    exit 1
fi

if [ ! -d "$WORK_DIR" ]; then
    echo "*** Error: wrong WORK_DIR: $WORK_DIR" >&2
    exit 1
fi

mkdir -p "$WORK_TMP_DIR"
if [ $? -ne 0 ]; then
    echo "*** Error: failed to mkdir: $WORK_TMP_DIR" >&2
    exit 1
fi

extract_ipk.sh "$HMI_IPK_FILE_PATH" "$WORK_TMP_DIR"

echo "All of the files are extract into $WORK_TMP_DIR."
echo "You can execute in gdb command line: set sysroot ~/work/dev_env/gwm_new/SyDK/NAC_CN/imx6x-std-adk/armv7a-mv-linux"
find `basename $WORK_TMP_DIR` -name "hmi-application*"
echo "You can execute command line like: arm-none-linux-gnueabi-gdb -c core -e tmp/usr/bin/ovip/HMI/hmi-application -s tmp/armv7a-mv-linux/usr/bin/ovip/HMI/hmi-application.dbg"
