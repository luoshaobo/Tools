#!/bin/bash

IMAGE_FILES=`find -type f | tr " " "^"`

get_normal_path()
{
    local IMAGE_FILE
    
    IMAGE_FILE="$1"
    
    echo "$IMAGE_FILE" | tr "^" " "
}

for IMAGE_FILE in $IMAGE_FILES; do
    IMAGE_FILE_NORMAL=`get_normal_path "$IMAGE_FILE"`
    
    SIZE=`du -h "$IMAGE_FILE_NORMAL" | awk '{ print $1 }'`
    echo -e "$SIZE\t\t\t\t$IMAGE_FILE" | sed 's/\//\\/g'
done
