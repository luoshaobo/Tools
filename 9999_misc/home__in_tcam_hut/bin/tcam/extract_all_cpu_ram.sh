#!/bin/bash

export PATH=$HOME/bin/tcam:$PATH

INPUT_LOG_FILES=`find . -name "*.LAT2"`

for INPUT_LOG_FILE in $INPUT_LOG_FILES; do
    if [ -r "$INPUT_LOG_FILE" ]; then
        echo "extract cpu and ram from file: $INPUT_LOG_FILE ..."
        
        echo "  extract_cpu_info.sh \"$INPUT_LOG_FILE\""
        extract_cpu_info.sh "$INPUT_LOG_FILE"
        if [ $? -ne 0 ]; then
            exit
        fi
        
        echo "  extract_mem_info.sh \"$INPUT_LOG_FILE\""
        extract_mem_info.sh "$INPUT_LOG_FILE"
        if [ $? -ne 0 ]; then
            exit
        fi
        
        echo "  extract_ram_info.sh \"$INPUT_LOG_FILE\""
        extract_ram_info.sh "$INPUT_LOG_FILE"
        if [ $? -ne 0 ]; then
            exit
        fi
        
        echo "  extract_ram_voc_info.sh \"$INPUT_LOG_FILE\""
        extract_ram_voc_info.sh "$INPUT_LOG_FILE"
        if [ $? -ne 0 ]; then
            exit
        fi
        
        echo "  extract_ram_coma_info.sh \"$INPUT_LOG_FILE\""
        extract_ram_coma_info.sh "$INPUT_LOG_FILE"
        if [ $? -ne 0 ]; then
            exit
        fi
        
        echo "  extract_ram_ipcb_info.sh \"$INPUT_LOG_FILE\""
        extract_ram_ipcb_info.sh "$INPUT_LOG_FILE"
        if [ $? -ne 0 ]; then
            exit
        fi
        
        echo "done"
    fi
done
