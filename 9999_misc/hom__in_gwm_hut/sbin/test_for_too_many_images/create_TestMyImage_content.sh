#!/bin/sh

IMAGE_FILES=`(cd "/media/datastore/ui/resources/CYC/assets/images/ambiance_1" && find . -name "*_normal.png")`
HEAD_FILE_PATH="/home/root/sbin/xxx/TestMyImage_Head.qml"
TAIL_FILE_PATH="/home/root/sbin/xxx/TestMyImage_Tail.qml"

cat $HEAD_FILE_PATH

I=1
for IMAGE_FILE in $IMAGE_FILES; do
    IMAGE_FILE_BASE_NAME=${IMAGE_FILE%%_normal.png}
    LAST_I=`expr $I - 1`
    
    echo "        MyImage {";
    echo "            id: image_${I};";
    echo "            anchors.top: image_${LAST_I}.bottom;";
    echo "            baseName: imageBaseDir + \"/${IMAGE_FILE_BASE_NAME}\";";
    echo "        }";
    
    I=`expr $I + 1`
done

cat $TAIL_FILE_PATH
