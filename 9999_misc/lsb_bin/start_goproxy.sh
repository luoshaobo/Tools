#!/bin/bash

(
    cd /cygdrive/d/sw/00_installed/proxy-windows-amd64
    ./proxy sps --daemon -S http -T tcp -P 127.0.0.1:8080 -t tcp -p :10080 --disable-http
    ./proxy udp --daemon -p ":53" -T udp -P "8.8.8.8:53"
)