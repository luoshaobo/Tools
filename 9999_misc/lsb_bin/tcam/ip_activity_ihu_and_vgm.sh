#!/bin/bash

NOT_USED=${LOCAL_ADDR1:="198.18.32.1:50001"}
NOT_USED=${LOCAL_ADDR2:="198.18.34.15:50001"}

I=0
while :; do
    printf "[%05u]broadcast to 198.18.255.255:50001 from $LOCAL_ADDR1\n" $I
    printf "\xff\xff\xff\x01\x00\x00\x00\x0c\xff\x01\x06\xce\x03\x06\x01\x00\x01\x00\x00\x00" | socat - UDP-DATAGRAM:198.18.255.255:50001,broadcast,bind=$LOCAL_ADDR1
    printf "[%05u]broadcast to 198.18.255.255:50001 from $LOCAL_ADDR2\n" $I
    printf "\xff\xff\xff\x01\x00\x00\x00\x0c\xff\x01\x06\xce\x03\x06\x01\x00\x01\x00\x00\x00" | socat - UDP-DATAGRAM:198.18.255.255:50001,broadcast,bind=$LOCAL_ADDR2
    sleep 0.5
    I=$((I + 1))
done
