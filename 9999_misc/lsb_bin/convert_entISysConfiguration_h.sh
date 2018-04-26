#!/bin/bash

PATHNAME__SYSCONF_H='D:\casdev\WinCE\public\MMP_PROD\OAK\INC\_SYS\Configuration\entISysConfiguration.h'
PATHNAME__SYSCONF_USER_H='D:\casdev\WinCE\public\MMP_PROD\_SYS\Configuration\AccessLib\src\entISysConfiguration_user.h'

FUNCS_IMPLED=`cat <<EOF
entISysVf_GetMainPanBtnAccess(
entISysVf_SetAvpActivation(
entISysVf_SendAvpViewRequest(
entISysVf_GetAvpAccessStatus(
EOF`

function get_file_header()
{
    cat <<EOF
#ifndef _entISysConfiguration_user_h
#define _entISysConfiguration_user_h

#include <ssw_types.h>
#include "entISysConfiguration.h"


#define RET_NOT_PROCESSED                   -1

class HMISysConfigurationAccessLibUser
{
public:
    static HMISysConfigurationAccessLibUser& getInstance() {
        static HMISysConfigurationAccessLibUser instance;
        return instance;
    }
    
EOF
}

function get_file_tailer()
{
    cat <<EOF
};

#endif // #define _entISysConfiguration_user_h

EOF
}

function make_addtional_functions_declaration()
{
    echo "    HMISysConfigurationAccessLibUser();"
    echo "    ~HMISysConfigurationAccessLibUser();"
}

function get_impled_func_declaration()
{
    local CMD_LINE
    
    make_addtional_functions_declaration
    
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="cat \"$PATHNAME__SYSCONF_H\" | grep -e \"^[[:space:]]*\(void\|BOOL\|bool_t\)[[:space:]]\+\(entISysConfiguration_\|entISysVf_\)\" | grep \"$FUNC\" | sed -r 's/^[ \t]*(void|BOOL|bool_t)/int/g'"
        echo -n "    "
        #echo "$CMD_LINE" >&2
        eval "$CMD_LINE"
    done
}

function get_unimpled_func_declaration()
{
    local CMD_LINE
    
    CMD_LINE="cat \"$PATHNAME__SYSCONF_H\" | grep -e \"^[[:space:]]*\(void\|BOOL\|bool_t\)[[:space:]]\+\(entISysConfiguration_\|entISysVf_\)\" "
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="$CMD_LINE | grep -v \"$FUNC\""
    done
    CMD_LINE="$CMD_LINE | sed \"s/;/ { return RET_NOT_PROCESSED; }/g\""
    #echo "$CMD_LINE" >&2
    eval "$CMD_LINE" | sed -r 's/^[ \t]*(void|BOOL|bool_t)/int/g' | sed 's/^/    /g'
}

function main()
{
    get_file_header
    get_impled_func_declaration
    get_unimpled_func_declaration
    get_file_tailer
}

echo "Write file: $PATHNAME__SYSCONF_USER_H..."
main > "$PATHNAME__SYSCONF_USER_H"
echo "done!"
