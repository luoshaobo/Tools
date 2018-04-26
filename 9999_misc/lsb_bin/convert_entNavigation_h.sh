#!/bin/bash

PATHNAME__ENTNAVIGATION_H='D:\casdev\WinCE\public\MMP_PROD\_NAV\Main\AccessLib\inc\entNavigation.h'
PATHNAME__ENTNAVIGATION_USER_H='D:\casdev\WinCE\public\MMP_PROD\_NAV\Main\AccessLib\inc\entNavigation_user.h'

FUNCS_IMPLED=`cat <<EOF
command_GetPushObjectShortName(
command_GetPushObjectDetail(
command_GetPOIPushObjectDetailData(
command_GetGuidancePushObjectDetailData(
command_DeletePushObject(
command_SetPushObjectDynaMode(
command_GetPushObjectDynaMode(

command_GetFavouriteListStatus(
command_AddFavourite(

command_SetDestination(
command_StartRouteCalculation(
command_AbortRouteCalculation(

command_MoveMap(
command_StopMoveMap(
command_SelectPointOnMap(
command_SetCCPDisplay(
command_SetZoomLevel(
command_GetZoomLevel(

command_GetConnNavEnableSetting(

command_GetTrafficBargraph(
command_GetTrafficBargraphData(

command_GetTPEGMode(
command_SetTPEGMode(

command_StartNetworkSearch(
command_AbortNetworkSearch(
command_GetNetPOISearchElement(
command_GetNetPOISearchElementData(
command_GetNetPOISearchElementDetails(
command_GetNetPOISearchElementDetailsData(
command_GetNetParkingSearchElementDetailsData(
command_GetNetGasStationSearchElementDetailsData(

command_GetDefaultPoiSearchSetting(
command_SetDefaultPoiSearchSetting(

command_GetJourneyInfoData(

EOF`

function get_file_header()
{
    cat <<EOF
#ifndef _entNavigation_user_h
#define _entNavigation_user_h

#include <ssw_types.h>
#include "NavInterfaceDefinition.h"
#include "NavBaselineControl.h"

#define RET_FALSE                           0
#define RET_TRUE                            1
#define RET_NOT_PROCESSED                   2

class HMINavigationAccessLibUser
{
public:
    static HMINavigationAccessLibUser& getInstance() {
        static HMINavigationAccessLibUser instance;
        return instance;
    }
    
EOF
}

function get_file_tailer()
{
    cat <<EOF
};

#endif // #define _entNavigation_user_h

EOF
}

function make_addtional_functions_declaration()
{
    echo "    HMINavigationAccessLibUser();"
    echo "    ~HMINavigationAccessLibUser();"
}

function get_impled_func_declaration()
{
    local CMD_LINE
    
    make_addtional_functions_declaration
    
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="cat \"$PATHNAME__ENTNAVIGATION_H\" | grep -e \"^[[:space:]]*\(BOOL\|NAV_API_RET_VAL\)[[:space:]]\+command_\" | grep \"$FUNC\""
        #echo "$CMD_LINE" >&2
        eval "$CMD_LINE"
    done
}

function get_unimpled_func_declaration()
{
    local CMD_LINE
    
    CMD_LINE="cat \"$PATHNAME__ENTNAVIGATION_H\" | grep -e \"^[[:space:]]*\(BOOL\|NAV_API_RET_VAL\)[[:space:]]\+command_\" "
    for FUNC in $FUNCS_IMPLED; do
        if [ -z "$FUNC" ]; then
            continue
        fi
        CMD_LINE="$CMD_LINE | grep -v \"$FUNC\""
    done
    CMD_LINE="$CMD_LINE | sed \"s/;/ { return RET_NOT_PROCESSED; }/g\""
    #echo "$CMD_LINE" >&2
    eval "$CMD_LINE"
}

function main()
{
    get_file_header
    get_impled_func_declaration
    get_unimpled_func_declaration
    get_file_tailer
}

echo "Write file: $PATHNAME__ENTNAVIGATION_USER_H..."
main > "$PATHNAME__ENTNAVIGATION_USER_H"
echo "done!"
