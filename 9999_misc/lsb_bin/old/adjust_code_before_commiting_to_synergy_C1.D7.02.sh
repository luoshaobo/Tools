#!/bin/bash

CUR_SRC_ROOT="."
BAK_SRC_ROOT="/cygdrive/d/casdev/uidv4956/CCM_WA/DPCARSha_STEP2/MRN_STEP2_001/DPCA_iMX/Delivery/MMP_GENERIC/GEN_HS/src"

TMP_FILE="/tmp/temp_72378483247237.tmp"

####################################################################################################
## check $CUR_SRC_ROOT and $BAK_SRC_ROOT
##
if [ ! -d $CUR_SRC_ROOT/HMI -o ! -d $CUR_SRC_ROOT/HMI2 ]; then
    echo "*** Error: \"$CUR_SRC_ROOT\" is invalid!" >&2
    exit 1
fi

if [ ! -d $BAK_SRC_ROOT/HMI -o ! -d $BAK_SRC_ROOT/HMI2 ]; then
    echo "*** Error: \"$BAK_SRC_ROOT\" is invalid!" >&2
    exit 1
fi

####################################################################################################
## 
##
(cd .. && clean_release.sh)

####################################################################################################
## 
##
SRC_DIR_HMI2_CODEGEN_GUI_DPCA="$CUR_SRC_ROOT/HMI2/CodeGen/GUI_DPCA"
(
    cd $SRC_DIR_HMI2_CODEGEN_GUI_DPCA
    rm -fr backup font_files orig_makefile 00_create_makefile.sh 01_create_makefile.py 02_build_optimized_cpp.bat makefile.mak makefile.orig
)
BAK_DIR_HMI2_CODEGEN_GUI_DPCA="$BAK_SRC_ROOT/HMI2/CodeGen/GUI_DPCA"
cp -pf $BAK_DIR_HMI2_CODEGEN_GUI_DPCA/makefile $SRC_DIR_HMI2_CODEGEN_GUI_DPCA/makefile

####################################################################################################
## 
##
BAK_DIR_HMI_00_ADDITIONS="$CUR_SRC_ROOT/00-additions"
rm -fr $BAK_DIR_HMI_00_ADDITIONS











