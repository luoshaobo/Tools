#!/bin/bash

PATHNAME__ENTAVAUDIOVIDEO_H='D:\casdev\WinCE\public\MMP_PROD\OAK\INC\_AV\entIAvAudioVideo.h'
PATHNAME__ENTAVAUDIOVIDEO_USER_H='D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAudioVideo_user.h'

FUNCS_IMPLED=`cat <<EOF
entIAV_SetHiFiMode(
entIAV_GetHiFiMode(
entIAV_GetAudioSettingsAvailability(
entIAV_GetAasData(

entIAV_GetVRinProgressStatus(
entIAV_HMI_Event(
entIAV_GetVR_ResultPoiList(
entIAV_GetVR_Prompt_Setting(
entIAV_SetVR_Prompt_Setting(
entIAV_GetVR_Result(
EOF`

function get_file_header()
{
    cat <<EOF
#ifndef _entIAvAudioVideo_user_h
#define _entIAvAudioVideo_user_h

#include <ssw_types.h>
#include "entIAvAudioVideo.h"
#include "entIAVMSGQ.h"
#include "entIAvUMSGQ_def.h"
#include "entIAvVrExt.h"


#define RET_FALSE                           0
#define RET_TRUE                            1
#define RET_NOT_PROCESSED                   2

class HMIAvAudioVideoAccessLibUser
{
public:
    static HMIAvAudioVideoAccessLibUser& getInstance() {
        static HMIAvAudioVideoAccessLibUser instance;
        return instance;
    }
    
EOF
}

function get_file_tailer()
{
    cat <<EOF
};

#endif // #define _entIAvAudioVideo_user_h

EOF
}

function make_addtional_functions_declaration()
{
    echo "    HMIAvAudioVideoAccessLibUser();"
    echo "    ~HMIAvAudioVideoAccessLibUser();"
}

function get_impled_func_declaration()
{
    local CMD_LINE
    
    make_addtional_functions_declaration
    
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="cat \"$PATHNAME__ENTAVAUDIOVIDEO_H\" | grep -e \"^[[:space:]]*\(bool_t\)[[:space:]]\+entIAV_\" | grep \"$FUNC\""
        #echo "$CMD_LINE" >&2
        eval "$CMD_LINE" | sed 's/^/    /g' | sed 's/^    bool_t/    int/g'
    done
}

function get_unimpled_func_declaration()
{
    local CMD_LINE
    
    CMD_LINE="cat \"$PATHNAME__ENTAVAUDIOVIDEO_H\" | grep -e \"^[[:space:]]*\(bool_t\)[[:space:]]\+entIAV_\" "
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="$CMD_LINE | grep -v \"$FUNC\""
    done
    CMD_LINE="$CMD_LINE | sed \"s/;/ { return RET_NOT_PROCESSED; }/g\""
    #echo "$CMD_LINE" >&2
    eval "$CMD_LINE" | sed 's/^/    /g' | sed 's/^    bool_t/    int/g'
}

function main()
{
    get_file_header
    get_impled_func_declaration
    get_unimpled_func_declaration
    get_file_tailer
}

echo "Write file: $PATHNAME__ENTAVAUDIOVIDEO_USER_H..."
main > "$PATHNAME__ENTAVAUDIOVIDEO_USER_H"
echo "done!"
