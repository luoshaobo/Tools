#!/bin/bash

ADAPTOR_PATH_PREFIX='D:\casdev\WinCE\public\MMP_PROD\_HS'
LOG_FILE_PATH="d:/gen_vaps__HMI_DPCA__all.log"

CMD_LINE=`cat <<EOF
cmd /c "C:\Presagis\VAPS_XT_32\bin\CodeGen.exe" "$ADAPTOR_PATH_PREFIX\HMI2\HMI_DPCA.vrpj" -codegen GUI_DPCA.vimp 2>&1 | tee $LOG_FILE_PATH
EOF`
echo "$CMD_LINE"
eval $CMD_LINE

####################################################################################################

create_makefile()
{
    echo "=============================================================================================="
    (
        CMD_LINE=`cat <<EOF
cd "/cygdrive/d/casdev/WinCE/public/MMP_PROD/_HS/HMI2/CodeGen/GUI_DPCA"
EOF`
        echo "$CMD_LINE"
        eval $CMD_LINE
        
        CMD_LINE=`cat <<EOF
./01_create_makefile.py all
EOF`
        echo "$CMD_LINE"
        eval $CMD_LINE
    )
}

create_makefile 2>&1 | tee -a $LOG_FILE_PATH

####################################################################################################

extract_changed_vimps()
{
    echo "=============================================================================================="
    echo "***"
    echo "*** NOTE: these vimp files as below must be open-and-saved manually:"
    echo "***"
    ./extract_changed_vimps.sh
}

extract_changed_vimps 2>&1 | tee -a $LOG_FILE_PATH
