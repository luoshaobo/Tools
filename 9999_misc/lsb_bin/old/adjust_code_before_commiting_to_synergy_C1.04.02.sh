#!/bin/bash

SRC_ROOT="."
DST_ROOT="/cygdrive/d/casdev/my_snapshot/C1.04.02/DPCA_iMX/Delivery/MMP_GENERIC/GEN_HS/src"

TMP_FILE="/tmp/temp_72378483247237.tmp"

####################################################################################################
## check $SRC_ROOT and $DST_ROOT
##
if [ ! -d $SRC_ROOT/HMI -o ! -d $SRC_ROOT/HMI2 ]; then
    echo "*** Error: \"$SRC_ROOT\" is invalid!" >&2
    exit 1
fi

if [ ! -d $DST_ROOT/HMI -o ! -d $DST_ROOT/HMI2 ]; then
    echo "*** Error: \"$DST_ROOT\" is invalid!" >&2
    exit 1
fi

####################################################################################################
## 
##
SRC_FILE_HMI2_CONTROLS_TELEMATICS_DIRS="$SRC_ROOT/HMI2/Controls/Telematics/dirs"
cat $SRC_FILE_HMI2_CONTROLS_TELEMATICS_DIRS | grep -v "TlmDummy \\\\" > $TMP_FILE
cp -pf $TMP_FILE $SRC_FILE_HMI2_CONTROLS_TELEMATICS_DIRS

SRC_FILE_HMI2_CONTROLS_TELEMATICS_MAKEFILE_MK="$SRC_ROOT/HMI2/Controls/Telematics/makefile.mk"
cat $SRC_FILE_HMI2_CONTROLS_TELEMATICS_MAKEFILE_MK | sed -E 's/!if 1 # the on-off/!if 0 # the on-off/' > $TMP_FILE
cp -pf $TMP_FILE $SRC_FILE_HMI2_CONTROLS_TELEMATICS_MAKEFILE_MK

####################################################################################################
## 
##
SRC_DIR_HMI2_CODEGEN_GUI_DPCA="$SRC_ROOT/HMI2/CodeGen/GUI_DPCA"
(
    cd $SRC_DIR_HMI2_CODEGEN_GUI_DPCA
    rm -fr backup font_files orig_makefile 00_create_makefile.sh 01_create_makefile.py makefile.mak
)
DST_DIR_HMI2_CODEGEN_GUI_DPCA="$DST_ROOT/HMI2/CodeGen/GUI_DPCA"
cp -pf $DST_DIR_HMI2_CODEGEN_GUI_DPCA/makefile $SRC_DIR_HMI2_CODEGEN_GUI_DPCA/makefile

####################################################################################################
## 
##
SRC_FILE_HMI_DIRS="$SRC_ROOT/HMI/dirs"
DST_FILE_HMI_DIRS="$DST_ROOT/HMI/dirs"
cp -pf $DST_FILE_HMI_DIRS $SRC_FILE_HMI_DIRS

####################################################################################################
## 
##
DST_DIR_HMI2_ADDITIONAL_FILES="$SRC_ROOT/HMI2/addtional_files"
rm -fr $DST_DIR_HMI2_ADDITIONAL_FILES

