#!/bin/bash

ADK_INSTALLER_ROOT_DIR="$1"
WORK_DIR="$2"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <ADK_INSTALLER_ROOT_DIR> <WORK_DIR>"
    echo ""
}

ROOTFS_FILE_PATH="$ADK_INSTALLER_ROOT_DIR/ADK/NAC/sys-image-nac-cn-imx6x-std.tar.bz2"
ADK_INSTALL_FILE_PATH="$ADK_INSTALLER_ROOT_DIR/ADK/NAC/nac-sys-image-nac-cn-imx6x-std-adk.sh"
WORK_TMP_DIR="$WORK_DIR/tmp"

if [ $# -lt 2 ]; then
    usage
    exit
fi

if [ ! -f "$ROOTFS_FILE_PATH" -o ! -f "$ADK_INSTALL_FILE_PATH" ]; then
    echo "*** Error: wrong ADK_INSTALLER_ROOT_DIR: $ADK_INSTALLER_ROOT_DIR" >&2
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

(
   cd "$WORK_TMP_DIR" && tar jxf "$ROOTFS_FILE_PATH" ./usr/bin/ovip/HMI/hmi-application
)
if [ $? -ne 0 ]; then
    echo "*** Error: failed to extract: ./usr/bin/ovip/HMI/hmi-application" >&2
    exit 1
fi

ARCHIVE=`awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' "$ADK_INSTALL_FILE_PATH"`
tail -n+$ARCHIVE "$ADK_INSTALL_FILE_PATH" | tar zx -C "$WORK_TMP_DIR"

(
   cd "$WORK_TMP_DIR" && tar Jxf nac-sys-image-nac-cn-imx6x-std-adk.tar.xz armv7a-mv-linux/usr/bin/ovip/HMI/hmi-application.dbg
)
if [ $? -ne 0 ]; then
    echo "*** Error: failed to extract: armv7a-mv-linux/usr/bin/ovip/HMI/hmi-application.dbg" >&2
    exit 1
fi

echo "All of the files are extract into $WORK_TMP_DIR."
echo "You can execute in gdb command line: set sysroot ~/work/dev_env/gwm_new/SyDK/NAC_CN/imx6x-std-adk/armv7a-mv-linux"
echo "Executable file: $WORK_TMP_DIR/usr/bin/ovip/HMI/hmi-application"
echo "Symbol file: $WORK_TMP_DIR/armv7a-mv-linux/usr/bin/ovip/HMI/hmi-application.dbg"
echo "You can execute command line like: arm-none-linux-gnueabi-gdb -c core -e tmp/usr/bin/ovip/HMI/hmi-application -s tmp/armv7a-mv-linux/usr/bin/ovip/HMI/hmi-application.dbg"
