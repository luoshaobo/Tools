#!/bin/bash

(
    cd /usr/lib/x86_64-linux-gnu
    sudo ln -f -s /usr/local/mesa-17.0.7/lib/libGL.so.1.5.0 libGL.so.1
    sudo ln -f -s /usr/local/mesa-17.0.7/lib/libglapi.so.0.0.0 libglapi.so
    sudo ln -f -s /usr/local/mesa-17.0.7/lib/libglapi.so.0.0.0 libglapi.so.0
)

