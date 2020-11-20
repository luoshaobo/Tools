#!/bin/bash

echo "adb kill-server"
adb kill-server

DATE=`date "+%Y%m%d%H%M.%S"`
CMD="adb shell /bin/busybox date -s $DATE"
echo "$CMD"
eval "$CMD"

echo "PC time:   `date`"
echo "TCAM time: `adb shell /bin/busybox date`"
