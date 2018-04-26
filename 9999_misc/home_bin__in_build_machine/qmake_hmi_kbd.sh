#!/bin/bash

qmake -r ../SYS_HMI/hmi-keyboard/hmi-keyboard.pro CONFIG+=without_simulator CONFIG+=dlt_backend $@

(
    cd ../SYS_HMI/hmi-keyboard/keyboard-service/keyboard-service/asian-input-engine-adaptor/freestylus/dynamic/
    cp -pf libHWRecog.so.1.1 libHWRecog.so
)

(
    cd ../SYS_HMI/hmi-keyboard/keyboard-service/keyboard-service/asian-input-engine-adaptor/keyboard/dynamic
    cp -pf libKeyBoard.so.1.1 libKeyBoard.so   
)
