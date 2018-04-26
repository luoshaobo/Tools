#!/bin/bash

PATHNAME__ENTTELEMATICS_H='D:\casdev\WinCE\public\MMP_PROD\OAK\INC\_TELE\AccessLib\entITelematics.h'
PATHNAME__ENTTELEMATICS_USER_H='D:\casdev\WinCE\public\MMP_PROD\_TELE\AccessLib\src\entITelematics_user.h'

FUNCS_IMPLED=`cat <<EOF
entITelematicsGetDevStatus(
entITelematicsGetNetStatus(
entITelematicsGetAPInfo(
entITelematicsSetAPStatus(
EOF`

function get_file_header()
{
    cat <<EOF
#ifndef _entITelematics_user_h
#define _entITelematics_user_h

#include <ssw_types.h>
#include "entITelematics.h"


#define RET_NOT_PROCESSED                   (E_TLM_RESULT)-1

class HMITelematicsAccessLibUser
{
public:
    static HMITelematicsAccessLibUser& getInstance() {
        static HMITelematicsAccessLibUser instance;
        return instance;
    }
    
EOF
}

function get_file_tailer()
{
    cat <<EOF
};

#endif // #define _entITelematics_user_h

EOF
}

function make_addtional_functions_declaration()
{
    echo "    HMITelematicsAccessLibUser();"
    echo "    ~HMITelematicsAccessLibUser();"
}

function get_impled_func_declaration()
{
    local CMD_LINE
    
    make_addtional_functions_declaration
    
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="cat \"$PATHNAME__ENTTELEMATICS_H\" | grep -e \"^[[:space:]]*E_TLM_RESULT[[:space:]]\+entITele\" | grep \"$FUNC\""
        echo -n "    "
        #echo "$CMD_LINE" >&2
        eval "$CMD_LINE"
    done
}

function get_unimpled_func_declaration()
{
    local CMD_LINE
    
    CMD_LINE="cat \"$PATHNAME__ENTTELEMATICS_H\" | grep -e \"^[[:space:]]*E_TLM_RESULT[[:space:]]\+entITele\" "
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="$CMD_LINE | grep -v \"$FUNC\""
    done
    CMD_LINE="$CMD_LINE | sed \"s/;/ { return RET_NOT_PROCESSED; }/g\""
    #echo "$CMD_LINE" >&2
    eval "$CMD_LINE" | sed 's/^/    /g'
}

function main()
{
    get_file_header
    get_impled_func_declaration
    get_unimpled_func_declaration
    get_file_tailer
}

echo "Write file: $PATHNAME__ENTTELEMATICS_USER_H..."
main > "$PATHNAME__ENTTELEMATICS_USER_H"
echo "done!"
