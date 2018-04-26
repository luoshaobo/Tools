#!/bin/bash

ADAPTOR_PATH_PREFIX='D:\casdev\WinCE\public\MMP_PROD\_HS'
LOG_FILE_PATH="d:/adaptor_gen.log"

ADAPTOR_CONFIG_FILE_TMP_SUFFIX="gat"

ADAPTOR_CONFIG_FILES=`cat <<EOF
HMI\Controls\Util\StringSplitter\StringSplitter.vprj
HMI2\Controls\Telematics\TelematicsControls.vprj
EOF`

ADAPTOR_PATHES=`cat <<EOF
EvtID.vtyp
ScreenID.vtyp
UShortImage.vtyp

StringSplitter.vcls

TlmGenAlertPopupAdapter.vcls
TlmGenListPopupAdapter.vcls
TlmGenQuestionPopupAdapter.vcls
TlmPopupLayout.vcls

EOF`

function modify_one_config_file()
{
    local ADAPTOR_CONFIG_FILE
    
    ADAPTOR_CONFIG_FILE="$1"
    
    if [ -f "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX" ]; then
        echo "*** ERROR: $ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX exists!" >&2
        exit 1
    fi
    
    if [ ! -f "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE" ]; then
        echo "*** ERROR: $ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE not exists!" >&2
        exit 1
    fi
    
    mv -f "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE" "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX"
    
    grep -v ".vimp" "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX" > "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE"
}

function modify_config_files()
{
    local ADAPTOR_CONFIG_FILE
    
    for ADAPTOR_CONFIG_FILE in $ADAPTOR_CONFIG_FILES; do
    if [ ! -z "$ADAPTOR_CONFIG_FILE" ]; then
        echo "$ADAPTOR_CONFIG_FILE" | grep -E "^[ \t]*#" >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            modify_one_config_file "$ADAPTOR_CONFIG_FILE"
        fi
    fi
done
}

function restore_one_config_file()
{
    local ADAPTOR_CONFIG_FILE
    
    ADAPTOR_CONFIG_FILE="$1"
    
    if [ ! -f "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX" ]; then
        echo "*** WARNING: $ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX not exists!" >&2
        return
    fi
    
    mv -f "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE.$ADAPTOR_CONFIG_FILE_TMP_SUFFIX" "$ADAPTOR_PATH_PREFIX/$ADAPTOR_CONFIG_FILE"
}

function restore_config_files()
{
    local ADAPTOR_CONFIG_FILE
    
    for ADAPTOR_CONFIG_FILE in $ADAPTOR_CONFIG_FILES; do
    if [ ! -z "$ADAPTOR_CONFIG_FILE" ]; then
        echo "$ADAPTOR_CONFIG_FILE" | grep -E "^[ \t]*#" >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            restore_one_config_file "$ADAPTOR_CONFIG_FILE"
        fi
    fi
done
}

if [ $# -ge 1 -a "$1" = "restore" ]; then
    restore_config_files
    exit 0
fi

ALL_ADAPTORS=""
for ADAPTOR_PATH in $ADAPTOR_PATHES; do
    if [ ! -z "$ADAPTOR_PATH" ]; then
        echo "$ADAPTOR_PATH" | grep -E "^[ \t]*#" >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            ALL_ADAPTORS="$ALL_ADAPTORS $ADAPTOR_PATH"
        fi
    fi
done

if [ ! -z "$ALL_ADAPTORS" ]; then
    modify_config_files
    
    CMD_LINE=`cat <<EOF
cmd /c "C:\Presagis\VAPS_XT_32\bin\CodeGen.exe" "$ADAPTOR_PATH_PREFIX\HMI2\HMI_DPCA.vrpj" -codegen $ALL_ADAPTORS
EOF`
    echo "$CMD_LINE"
    eval $CMD_LINE
    
    restore_config_files
fi