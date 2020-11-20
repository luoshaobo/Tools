#!/bin/bash

TCAM_TARGET_ROOT_FS_DIR="/home/uidv4956/tcam_root/tcam_src_root.latest/release-toolchain/fs/devel"

if [ $# -ge 1 ]; then
    TCAM_TARGET_ROOT_FS_DIR="$1"
fi

HOSTNAME_FROM_LOCAL_ETC_HOSTNAME=`cat ./etc/hostname`
HOSTNAME_FROM_TARGET_ETC_HOSTNAME=`cat "$TCAM_TARGET_ROOT_FS_DIR/etc/hostname"`

function usage
{
    echo "Usage:"
    echo "    `basename $0` <remote_target_root_dir>"
}

#if [ $# -lt 1 ]; then
#    usage
#    exit 1
#fi

if [ "$HOSTNAME_FROM_LOCAL_ETC_HOSTNAME" != "gly-tcam" ]; then
    echo "*** ERROR: the local direcory is not a tcam root fs directory!" >&2
    exit 1
fi

#if [ "$HOSTNAME_FROM_TARGET_ETC_HOSTNAME" != "gly-tcam" ]; then
#    echo "*** ERROR: the remote direcory is not a tcam root fs directory!" >&2
#    exit 1
#fi

#if [ ! -f "$TCAM_TARGET_ROOT_FS_DIR/SmeDbgLog.txt" -a ! -h "$TCAM_TARGET_ROOT_FS_DIR/SmeDbgLog.txt" ]; then
#    echo "*** ERROR: the remote direcory is not a tcam root fs directory!" >&2
#    exit 1
#fi

FILES=`cat <<EOF
usr/lib/libstdc++.so.6
lib/libm.so.6
lib/librt.so.1
lib/libpthread.so.0
lib/libgcc_s.so.1
lib/libc.so.6
lib/ld-linux-armhf.so.3
lib/libdl.so.2
lib/libnss_compat.so.2
lib/libnsl.so.1
lib/libnss_nis.so.2
lib/libnss_files.so.2
lib/libresolv.so.2
EOF`

for FILE in $FILES; do
    #echo "$FILE"
    #ls -l "$TCAM_TARGET_ROOT_FS_DIR/$FILE"
    
    #if [ ! -f "./$FILE" -a ! -h "./$FILE" ]; then
        echo "$FILE"
        cp -fL "$TCAM_TARGET_ROOT_FS_DIR/$FILE" "./$FILE"
    #fi
done
