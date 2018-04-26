#!/bin/bash

COUNT=384000
I=0


>SplashScreen.fb
CONTENT=""
SLICE="\\xFF\\x00\\x00\\xFF"
while [ $I -lt $COUNT ]; do
    #echo -n -e "\xFF\x00\x00\xFF" >> SplashScreen.fb
    
    CONTENT="$CONTENT$SLICE"
    
    I=`expr $I + 1`
done

echo -n -e "$CONTENT" >> SplashScreen.fb
