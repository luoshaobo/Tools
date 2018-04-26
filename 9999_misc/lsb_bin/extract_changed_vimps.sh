#!/bin/bash

INPUT_LOG_FILE_PATH="d:/gen_vaps__HMI_DPCA__all.log"

FILEPATHS=`cat $INPUT_LOG_FILE_PATH | grep -e "^! Warning(s) in file" | grep -e "\.vimp':$" | tr -d "!\'\(\)" | tr " " "_" | sed -e 's/_Warnings_in_file_//g' | sed -e 's/:$//g'`


#echo $FILEPATHS

I=0
for FILEPATH in $FILEPATHS; do
    echo $FILEPATH    
    I=`expr $I + 1`
done
echo "### Totally $I files changed."

# for FILEPATH in $FILEPATHS; do   
    # FILEDIR=`dirname $FILEPATH`
    # FILENAMEEXT=`basename $FILEPATH`
    # FILENAME=`basename $FILEPATH .vimp`
    
    # echo "  <file name=\"$FILENAME.vcls\" path=\"$FILEDIR\"/>"
    # echo "  <file name=\"$FILENAME.vimp\" path=\"$FILEDIR\"/>"
    # echo ""
# done

