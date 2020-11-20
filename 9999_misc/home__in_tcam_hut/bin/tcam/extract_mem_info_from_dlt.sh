#!/bin/bash

DLT_FILES=`ls *.dlt`
RESULT_DIR="results"

mkdir -p "$RESULT_DIR"

for DLT_FILE in $DLT_FILES; do
    echo "### begin to process: $DLT_FILE"

    MEM_AVAI_RESULT_FILE="$RESULT_DIR/MEM_AVAIL__${DLT_FILE}.txt"
    dlt-convert -a "$DLT_FILE" | grep "MEMI MEMI" | grep "MemAvai" > "$MEM_AVAI_RESULT_FILE"
    
    VMEM_VOC_RESULT_FILE="$RESULT_DIR/VMEM_VOC__${DLT_FILE}.txt"
    dlt-convert -a "$DLT_FILE" | grep "TOPM MEM" | grep "voc" > "$VMEM_VOC_RESULT_FILE"
    
    VMEM_COMA_RESULT_FILE="$RESULT_DIR/VMEM_COMA__${DLT_FILE}.txt"
    dlt-convert -a "$DLT_FILE" | grep "TOPM MEM" | grep "coma-core" > "$VMEM_COMA_RESULT_FILE"
    
    echo "### end to process: $DLT_FILE"
done
