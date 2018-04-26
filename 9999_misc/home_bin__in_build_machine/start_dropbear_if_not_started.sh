#!/bin/bash

N=`ps aux | grep dropbear_rsa_host_key | grep -v "grep" | wc -l`
#echo $N

if [ -z "$N" -o "$N" -lt 1 ]; then
    echo "Start dropbear!"
    ~/bin/start_dropbear.sh
else
    echo "Dropbear has been started!"
fi
