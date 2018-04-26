#!/bin/bash

IDS=`ipcs -q | awk -F" " '{ print $2 }'`
for ID in $IDS; do 
    ipcrm -q $ID
done
