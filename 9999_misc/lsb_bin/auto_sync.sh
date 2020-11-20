#/bin/bash

SLEEP_SEC=60

if [ $# -ge 1 ]; then
    SLEEP_SEC="$1"
fi

while `true`; do
    sync
    echo "sleep $SLEEP_SEC"
    sleep $SLEEP_SEC
done
