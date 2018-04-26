#!/bin/bash

make -j $@ \
&& cp -f hmi-application/hmi-application-131b hmi-application/hmi-application.dbg \
&& arm-none-linux-gnueabi-strip hmi-application/hmi-application-131b \

