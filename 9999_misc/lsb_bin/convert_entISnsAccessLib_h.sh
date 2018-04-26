#!/bin/bash

PATHNAME__SNS_H='D:\casdev\WinCE\public\MMP_PROD\OAK\INC\_NAV\NavSNS\entISnsAccessLib.h'
PATHNAME__SNS_USER_H='D:\casdev\WinCE\public\MMP_PROD\_NAV\NavSNS\Main\AccessLib\src\entISnsAccessLib_user.h'

FUNCS_IMPLED=`cat <<EOF
EOF`

function get_file_header()
{
    cat <<EOF
#ifndef _entISnsAccessLib_user_h
#define _entISnsAccessLib_user_h

#include <ssw_types.h>
#include "entISnsAccessLib.h"


#ifndef RET_NOT_PROCESSED
#define RET_NOT_PROCESSED                   -1
#endif // #ifndef RET_NOT_PROCESSED

class HMIentISnsAccessLibUser
{
public:
    static HMIentISnsAccessLibUser& getInstance() {
        static HMIentISnsAccessLibUser instance;
        return instance;
    }
    
EOF
}

function get_file_tailer()
{
    cat <<EOF
};

#endif // #define _entISnsAccessLib_user_h

EOF
}

function make_addtional_functions_declaration()
{
    echo "    HMIentISnsAccessLibUser();"
    echo "    ~HMIentISnsAccessLibUser();"
}

function get_impled_func_declaration()
{
    local CMD_LINE
    
    make_addtional_functions_declaration
    echo ""
    
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="cat \"$PATHNAME__SNS_H\" | grep -e \"^[[:space:]]*\(SNS_RESULT_E\)[[:space:]]\+\(entISns\)\" | grep \"$FUNC\" | sed -r 's/^[ \t]*(SNS_RESULT_E)/int/g'"
        echo -n "    "
        #echo "$CMD_LINE" >&2
        eval "$CMD_LINE"
    done
}

function get_unimpled_func_declaration()
{
    local CMD_LINE
    
    CMD_LINE="cat \"$PATHNAME__SNS_H\" | grep -e \"^[[:space:]]*\(SNS_RESULT_E\)[[:space:]]\+\(entISns\)\" "
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="$CMD_LINE | grep -v \"$FUNC\""
    done
    CMD_LINE="$CMD_LINE | sed \"s/;/ { return RET_NOT_PROCESSED; }/g\""
    #echo "$CMD_LINE" >&2
    eval "$CMD_LINE" | sed -r 's/^[ \t]*(SNS_RESULT_E)/int/g' | sed 's/^/    /g'
}

function main()
{
    get_file_header
    get_impled_func_declaration
    get_unimpled_func_declaration
    get_file_tailer
}

echo "Write file: $PATHNAME__SNS_USER_H..."
main > "$PATHNAME__SNS_USER_H"
echo "done!"
