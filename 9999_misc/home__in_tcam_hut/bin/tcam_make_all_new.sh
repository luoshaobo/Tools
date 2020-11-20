#!/bin/bash

#OPT_VERSION=${OPT_VERSION:-1.96.0}              # 2.42
#OPT_VERSION=${OPT_VERSION:-1.116.0 }            # 2.45
#OPT_VERSION=${OPT_VERSION:-1.127.0}             # 2.50
OPT_VERSION=${OPT_VERSION:-1.127.0}             # 2.50.2.1

VAR_IMAGE_ONLY=0

echo "!!! opt-host-$OPT_VERSION will be used !!!"
sleep 5

if [ $# -ge 1 -a "x$1" = "xinit" ]; then
    echo "=== make clean ==="
    sleep 5
    make clean

    echo "=== make M=1 bootstrap CAS_TARGET_HW=gly-tcam V=1 J=32 TOOLCHAIN_VERSION=otp-host-${OPT_VERSION} ==="
    sleep 5
    make M=1 bootstrap CAS_TARGET_HW=gly-tcam V=1 J=32 TOOLCHAIN_VERSION=otp-host-${OPT_VERSION}

    echo "== make M=1 clobber CAS_TARGET_HW=gly-tcam =="
    make M=1 clobber CAS_TARGET_HW=gly-tcam
fi

if [ $# -ge 1 -a "x$1" = "ximage" ]; then
    VAR_IMAGE_ONLY=1
fi

if [ $VAR_IMAGE_ONLY -ne 1 ]; then
    echo "=== make V=1 J=32 CAS_TARGET_HW=gly-tcam ==="
    sleep 5
    make V=1 J=32 CAS_TARGET_HW=gly-tcam
fi

echo "=== make CAS_RWFS=50 image ==="
sleep 5
#make CAS_RWFS=50 image
make CAS_RWFS=5 image
