#!/bin/bash

CUR_SRC_ROOT="."
BAK_SRC_ROOT="/cygdrive/d/casdev/my_snapshot/C1.B5.02/DPCA_iMX/Delivery/MMP_GENERIC/GEN_HS/src"

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
CUR_FILE_HMI2_CONTROLS_TELEMATICS_DIRS="$CUR_SRC_ROOT/HMI2/Controls/Telematics/dirs"
cat $CUR_FILE_HMI2_CONTROLS_TELEMATICS_DIRS | grep -v "TlmDummy \\\\" > $TMP_FILE
cp -pf $TMP_FILE $CUR_FILE_HMI2_CONTROLS_TELEMATICS_DIRS

CUR_FILE_HMI2_CONTROLS_TELEMATICS_MAKEFILE_MK="$CUR_SRC_ROOT/HMI2/Controls/Telematics/makefile.mk"
cat $CUR_FILE_HMI2_CONTROLS_TELEMATICS_MAKEFILE_MK | sed -E 's/!if 1 # the on-off/!if 0 # the on-off/' > $TMP_FILE
cp -pf $TMP_FILE $CUR_FILE_HMI2_CONTROLS_TELEMATICS_MAKEFILE_MK

####################################################################################################
## 
##
SRC_DIR_HMI2_CODEGEN_GUI_DPCA="$CUR_SRC_ROOT/HMI2/CodeGen/GUI_DPCA"
(
    cd $SRC_DIR_HMI2_CODEGEN_GUI_DPCA
    rm -fr backup font_files orig_makefile 00_create_makefile.sh 01_create_makefile.py makefile.mak
)
BAK_DIR_HMI2_CODEGEN_GUI_DPCA="$BAK_SRC_ROOT/HMI2/CodeGen/GUI_DPCA"
cp -pf $BAK_DIR_HMI2_CODEGEN_GUI_DPCA/makefile $SRC_DIR_HMI2_CODEGEN_GUI_DPCA/makefile

####################################################################################################
## 
##
CUR_FILE_HMI_DIRS="$CUR_SRC_ROOT/HMI/dirs"
BAK_FILE_HMI_DIRS="$BAK_SRC_ROOT/HMI/dirs"
cp -pf $BAK_FILE_HMI_DIRS $CUR_FILE_HMI_DIRS

####################################################################################################
## 
##
CUR_DIR_HMI2_ADDITIONAL_FILES="$CUR_SRC_ROOT/HMI2/addtional_files"
rm -fr $CUR_DIR_HMI2_ADDITIONAL_FILES

####################################################################################################
## 
##
CUR_FILE_HMI_INFRASTRUCTURE_HMIMANAGER_SRC_MAKE_RELEASE_VXTCLSGUISCREEN_DLL="$CUR_SRC_ROOT/HMI/Infrastructure/HMIManager/src/make/Release/vxtClsGUIScreen.dll"
BAK_FILE_HMI_INFRASTRUCTURE_HMIMANAGER_SRC_MAKE_RELEASE_VXTCLSGUISCREEN_DLL="$BAK_SRC_ROOT/HMI/Infrastructure/HMIManager/src/make/Release/vxtClsGUIScreen.dll"
cp -pf $BAK_FILE_HMI_INFRASTRUCTURE_HMIMANAGER_SRC_MAKE_RELEASE_VXTCLSGUISCREEN_DLL $CUR_FILE_HMI_INFRASTRUCTURE_HMIMANAGER_SRC_MAKE_RELEASE_VXTCLSGUISCREEN_DLL

####################################################################################################
## 
##
BAK_DIR_HMI_00_ADDITIONS="$CUR_SRC_ROOT/00-additions"
rm -fr $BAK_DIR_HMI_00_ADDITIONS

####################################################################################################
## all *.vcproj
##
CUR_VCPROJ="$CUR_SRC_ROOT/HMI/Infrastructure/DataTypes/EvtID/make/vxtTypEvtID.vcproj"
BAK_VCPROJ="$BAK_SRC_ROOT/HMI/Infrastructure/DataTypes/EvtID/make/vxtTypEvtID.vcproj"
cp -pf $BAK_VCPROJ $CUR_VCPROJ

CUR_VCPROJ="$CUR_SRC_ROOT/HMI/Infrastructure/DataTypes/ScreenID/make/vxtTypScreenID.vcproj"
BAK_VCPROJ="$BAK_SRC_ROOT/HMI/Infrastructure/DataTypes/ScreenID/make/vxtTypScreenID.vcproj"
cp -pf $BAK_VCPROJ $CUR_VCPROJ

CUR_VCPROJ="$CUR_SRC_ROOT/HMI/Toolbox_DPCA/UShortImage/make/vxtTypUShortImage.vcproj"
BAK_VCPROJ="$BAK_SRC_ROOT/HMI/Toolbox_DPCA/UShortImage/make/vxtTypUShortImage.vcproj"
cp -pf $BAK_VCPROJ $CUR_VCPROJ











