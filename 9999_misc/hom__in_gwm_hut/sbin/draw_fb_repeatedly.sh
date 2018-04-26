#!/bin/sh

while [ true ]; do
    cat fb0.data.argb32 > /dev/fb0
    usleep 50000
done
