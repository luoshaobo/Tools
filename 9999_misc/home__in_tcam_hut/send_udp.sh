#!/bin/sh

if [ -z "$DEST_ADDR_PORT" ]; then
    DEST_ADDR_PORT="127.0.0.1:5555"
fi

echo "$@" | socat - udp4-datagram:$DEST_ADDR_PORT
