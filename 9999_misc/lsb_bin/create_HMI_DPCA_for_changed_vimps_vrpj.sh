#!/bin/bash

INPUT_LOG_FILE_PATH="d:/gen_vaps__HMI_DPCA__all.log"

FILEPATHS=`cat $INPUT_LOG_FILE_PATH | grep -e "^! Warning(s) in file" | grep -e "\.vimp':$" | tr -d "!\'\(\)" | tr " " "_" | sed -e 's/_Warnings_in_file_//g' | sed -e 's/:$//g'`

is_vimp_Widgets()
{
    local VIMP_FILE_NAME
    
    VIMP_FILE_NAME="$1"
    
    if [ -z "$VIMP_FILE_NAME" ]; then
        return 0
    fi
    
    find "D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Widgets" -iname "$VIMP_FILE_NAME"| grep "$VIMP_FILE_NAME" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        return 1
    fi
    
    find "D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Widgets" -iname "$VIMP_FILE_NAME"| grep "$VIMP_FILE_NAME" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        return 1
    fi
    
    return 0
}

generate_changed_vimp_files()
{
    local FILEPATH
    
    for FILEPATH in $FILEPATHS; do   
        FILEDIR=`dirname $FILEPATH`
        FILENAMEEXT=`basename $FILEPATH`
        FILENAME=`basename $FILEPATH .vimp`
        
        is_vimp_Widgets "$FILENAME.vimp"
        if [ $? -eq 0 ]; then
            echo "  <file name=\"$FILENAME.vcls\" path=\"$FILEDIR\"/>"
            echo "  <file name=\"$FILENAME.vimp\" path=\"$FILEDIR\"/>"
        else
            echo "  <!-- <file name=\"$FILENAME.vcls\" path=\"$FILEDIR\"/> -->"
            echo "  <!-- <file name=\"$FILENAME.vimp\" path=\"$FILEDIR\"/> -->"
        fi
        echo ""
    done
}

generate_vrpj()
{
    local TMPFILE
    
    TMPFILE=`mktemp`
    
    cat "D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\HMI_DPCA.vrpj" | tr "[ ]" "[^]" > $TMPFILE
    
    (while read -r LINEBUF; do
        (echo "$LINEBUF"  | tr "[^]" "[ ]" | grep 'GUI.vprj') >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo "  <!-- <file name=\"GUI.vprj\" path=\".\\GUI\"/> -->"
            echo ""
            generate_changed_vimp_files
        else
            (echo "$LINEBUF"  | tr "[^]" "[ ]" | grep 'HMI_DPCA.vrpj') >/dev/null 2>&1
            if [ $? -eq 0 ]; then
                echo "<project name=\"HMI_DPCA_for_changed_vimps.vrpj\">"
            else
                echo "$LINEBUF" | tr "[^]" "[ ]"
            fi
        fi
    done) < $TMPFILE
    
    rm -f $TMPFILE
}

generate_vrpj | tee "D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\HMI_DPCA_for_changed_vimps.vrpj"

echo ""
echo "*** Execute this command line in cmd.exe to open the project:"
echo "vapsxt D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\HMI_DPCA_for_changed_vimps.vrpj"
