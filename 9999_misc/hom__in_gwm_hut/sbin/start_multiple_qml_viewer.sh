#!/bin/sh

INSTANCE_COUNT=1

usage()
{
    echo "Usage:"
    echo "    `basename $0` <instance_count>"
    echo ""
}

if [ $# -lt 1 ]; then
    usage
fi

INSTANCE_COUNT="$1"
#echo "\$INSTANCE_COUNT=$INSTANCE_COUNT"

I=0
while [ $I -lt $INSTANCE_COUNT ]; do
    #echo "\$I=$I"
    echo "qml_viewer -c red -z 1 -t test_img ~/sbin/test_image.qml &"
    qml_viewer -c red -z 1 -t test_img ~/sbin/test_image.qml &
    I=`expr $I + 1`
done
