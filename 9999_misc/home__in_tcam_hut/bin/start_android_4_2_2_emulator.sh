#!/bin/bash

. ~/bin/set_android_4_2_2_env.sh
emulator -qemu -nand system,size=0x1f400000,file=$HOME/.android/avd/first.avd/android_4_2_2/system.img
