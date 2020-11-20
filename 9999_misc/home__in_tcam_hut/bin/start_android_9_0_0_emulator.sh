#!/bin/bash

IMAGE_DIR="/home/uidv4956/000_software/android-9.0.0_r22/out/target/product/generic"

. ~/bin/set_android_9_0_0_env.sh
#emulator -qemu -nand system,size=0x2f400000,file=$IMAGE_DIR/system.img
emulator 
