#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# NOTE:
#     please execute this program in Chinese environment (by using AppLocale: Simple Chinese)

import sys
import os
import string

NEW_LINE = "\r\n"

# NOTE:
# 1)  start_id == -1: the string ID is auto allocated.
# 2)  start_id == -2: the string ID is from ScreenID.vtyp with then same name.
# 2)  start_id == other: the string ID is the given value.

oLocalTextItemList = [
    {
        "start_id" : 1600,
    }, 
    {
        "start_id"  : -1,
        "name"      : "RETRY",
        "en"        : "retry",
        "ch"        : "重试",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "LocalSearch",
        "en"        : "Local Search",
        "ch"        : "本地搜索",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "LocalSearch2",
        "en"        : "Local Search",
        "ch"        : "本地搜索",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "LocalSearch3",
        "en"        : "Local",
        "ch"        : "本地搜索",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NetworkSeach",
        "en"        : "Network Seach",
        "ch"        : "网络搜索",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NetworkSeach2",
        "en"        : "Network Search",
        "ch"        : "网络搜索",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NetworkSeach3",
        "en"        : "Network",
        "ch"        : "网络搜索",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "Retry2",
        "en"        : "Try Again",
        "ch"        : "重新搜索",
        "fr"        : "",
    },
    
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_LocalSearchNoResult",
        "en"        : "No result on local search.",
        "ch"        : "本地搜索无结果。",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_NetworkSearchNoResult",
        "en"        : "No result on network search.",
        "ch"        : "网络搜索无结果。",
        "fr"        : "",
    },
    
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_NameTemplate",
        "en"        : "%s",
        "ch"        : "%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ProvinceTemplate",
        "en"        : "%s",
        "ch"        : "%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_CityTemplate",
        "en"        : "%s",
        "ch"        : "%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_TownTemplate",
        "en"        : "%s",
        "ch"        : "%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_AddressTemplate",
        "en"        : "Address: %s",
        "ch"        : "地址：%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_TelTemplate",
        "en"        : "Tel: %s",
        "ch"        : "电话：%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingStatusTemplate",
        "en"        : "Parking Status: %s",
        "ch"        : "停车场状态：%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingPriceTemplate",
        "en"        : "Price: %f",
        "ch"        : "价格：%f",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingChangeTypeTemplate",
        "en"        : "Change Type: %s",
        "ch"        : "收费类型：%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingChangeInfoTemplate",
        "en"        : "Change Info: %s",
        "ch"        : "收费信息：%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingDescTemplate",
        "en"        : "%s",
        "ch"        : "%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_GasNameTemplate",
        "en"        : "Gas Name: %s",
        "ch"        : "汽油名称：%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_GasPriceTemplate",
        "en"        : "Price: %f",
        "ch"        : "价格：%f",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_GasDescTemplate",
        "en"        : "%s",
        "ch"        : "%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_GlobalNetworkSearch",
        "en"        : "Network Search On Default",
        "ch"        : "默认为网络搜索",
        "fr"        : "",
    },
    
    # {{{ ParkingStatus
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingStatusIdle",
        "en"        : "idle",
        "ch"        : "空闲",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingStatusBusy",
        "en"        : "all",
        "ch"        : "繁忙",
        "fr"        : "",
    },
    # ParkingStatus }}}
    # {{{ ParkingChargeType
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingChargeTypeUnknown",
        "en"        : "unknown",
        "ch"        : "未知",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingChargeTypeFree",
        "en"        : "free",
        "ch"        : "免费",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingChargeTypeDay",
        "en"        : "day",
        "ch"        : "按天",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_NPOI_ParkingChargeTypeTiming",
        "en"        : "timing",
        "ch"        : "计时",
        "fr"        : "",
    },
    # ParkingChargeType }}}
    
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_NPOI_RETRY",
        "en"        : "Searching overtime",
        "ch"        : "搜索失败",
        "fr"        : "",
    },
]

####################################################################################################
## common
##

FILE_PATH__SCREENID_VTYP                                = """D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Infrastructure\DataTypes\ScreenID.vtyp"""
FILE_PATH__LOCALIZEDTEXTTABLE_VTBL                      = """D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Tables\Localization\LocalizedTextTable.vtbl"""

SHELL_CMD_TEMPLATE__FIND_SCREEN_ID                      = """grep "%s" '%s' | sed 's/^[ \t]*<named_value name="[^"]\+"[ \t]\+value="\([0-9]\+\)"[ \t]*\/>[ \t]*$/\\1/g'"""
SHELL_CMD_TEMPLATE__FIND_STR_UUID                       = """grep "name=\\"%s\\"" '%s' | sed 's/^[ \t]*<object name="[^"]\+" class="LocalizedTextTableElement" guid="\([^"]\+\)"[ \t]*>$/\\1/g'"""

def find_screen_id(strScreenName):
    txtCmd = SHELL_CMD_TEMPLATE__FIND_SCREEN_ID % (strScreenName, FILE_PATH__SCREENID_VTYP);
    #sys.stderr.write("%s%s" % (txtCmd, NEW_LINE));
    return get_list_from_shell_command(txtCmd);
    
def find_string_uuid(strStrName):
    txtCmd = SHELL_CMD_TEMPLATE__FIND_STR_UUID % (strStrName, FILE_PATH__LOCALIZEDTEXTTABLE_VTBL);
    #sys.stderr.write("%s%s" % (txtCmd, NEW_LINE));
    return get_list_from_shell_command(txtCmd);

def write_str_to_file(strContent, strFilePath):
    oFile = file(strFilePath, "wb");
    if oFile != None:
        oFile.write(strContent);
    oFile = None;

def get_list_from_shell_command(strShellCommand):
    oList = []
    oFile = os.popen(strShellCommand, "r");
    if oFile != None:
        while True:
            strLine = oFile.readline();
            if strLine == "":
                break;
            strLine = strLine.strip(" \t\r\n");
            oList.append(strLine);
    oFile = None;
    return oList;

def gen_uuid():
    oResultList = get_list_from_shell_command("uuidgen");
    return oResultList[0];
    
def get_uuid(strStrName):
    lRet = find_string_uuid(strStrName)
    if (len(lRet) == 0):
        sys.stderr.write("### INFO: can't find the string name: %s%s" % (strStrName, NEW_LINE));
        uuid = gen_uuid();
    else:
        uuid = lRet[0];
    return uuid;

def main():
    sResultStr = "";
    nStartId = 0;
    for oLocalTextItem in oLocalTextItemList:
        if oLocalTextItem["start_id"] == -1:
            nStartId += 1;
        elif oLocalTextItem["start_id"] == -2:
            lRet = find_screen_id(oLocalTextItem["name"])
            if (len(lRet) == 0):
                sys.stderr.write("*** ERROR: can't find the screen name: %s%s" % (oLocalTextItem["name"], NEW_LINE));
                exit(-1);
            else:
                nStartId = string.atoi(lRet[0]);
                #sys.stderr.write("------------%s+++++++++++%s" % (lRet[0], NEW_LINE));
        else:
            nStartId = oLocalTextItem["start_id"] - 1;
            if not oLocalTextItem.has_key("name"):
                continue;
            else:
                nStartId += 1;
        
        sResultStr += "  <object name=\"%s\" class=\"LocalizedTextTableElement\" guid=\"%s\">%s" % (oLocalTextItem["name"], get_uuid(oLocalTextItem["name"]), NEW_LINE);
        sResultStr += "    <model>%s" % (NEW_LINE);
        sResultStr += "      <prop name=\"StringId\">%d</prop>%s" % (nStartId, NEW_LINE);
        sResultStr += "      <arrayprop name=\"Element\" capacity=\"3\" size=\"3\">%s" % (NEW_LINE);
        
        sResultStr += "        <structentry>%s" % (NEW_LINE);
        sResultStr += "          <field name=\"Language\">en</field>%s" % (NEW_LINE);
        sResultStr += "          <field name=\"Value\">%s</field>%s" % (oLocalTextItem["en"], NEW_LINE);
        sResultStr += "        </structentry>%s" % (NEW_LINE);
        
        sResultStr += "        <structentry>%s" % (NEW_LINE);
        sResultStr += "          <field name=\"Language\">fr</field>%s" % (NEW_LINE);
        sResultStr += "          <field name=\"Value\">%s</field>%s" % (oLocalTextItem["fr"], NEW_LINE);
        sResultStr += "        </structentry>%s" % (NEW_LINE);
        
        sResultStr += "        <structentry>%s" % (NEW_LINE);
        sResultStr += "          <field name=\"Language\">ch</field>%s" % (NEW_LINE);
        sResultStr += "          <field name=\"Value\">%s</field>%s" % (oLocalTextItem["ch"], NEW_LINE);
        sResultStr += "        </structentry>%s" % (NEW_LINE);
        
        sResultStr += "      </arrayprop>%s" % (NEW_LINE);
        sResultStr += "    </model>%s" % (NEW_LINE);
        sResultStr += "  </object>%s" % (NEW_LINE);
        
    print sResultStr
    pass
        
            
        


if __name__ == "__main__":
    main();
    #print "哈哈";
