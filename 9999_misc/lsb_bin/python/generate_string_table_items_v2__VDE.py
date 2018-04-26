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
        "start_id" : 1800,
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_EXIT_Info",
        "en"        : "Exiting Voice Recognition",
        "ch"        : "�����˳���������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_MAIN_MENU_Info",
        "en"        : "What kind of help do you need?",
        "ch"        : "����Ҫʲô������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_KEYWORD_INPUT_Info",
        "en"        : "Please say Destination\\X000D\\X000AFor example The People's square/538 Dalian Road",
        "ch"        : "��˵Ŀ�ĵ�\\X000D\\X000A�磺����㳡/����·538��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_KEYWORD_LIST_title",
        "en"        : "Please select the keyword",
        "ch"        : "��ѡ��ؼ���",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_KEYWORD_CONFIRM_Info",
        "en"        : "The selected keyword: \\X000D\\X000A%s",
        "ch"        : "��ѡ�еĹؼ��֣�\\X000D\\X000A%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_POI_LIST_title",
        "en"        : "Search results",
        "ch"        : "�������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_POI_CONFIRM_Info",
        "en"        : "%s\\X000D\\X000A%s",
        "ch"        : "%s\\X000D\\X000A%s",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_POI_START_GUIDANCE",
        "en"        : "Go",
        "ch"        : "����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_MAIN_MENU_WITH_PHONE_CONNECTED_Info",
        "en"        : 'Please say "go to destination name" like "go to Tiananmen Square", "call name", "dial number" or "news", "weather".',
        "ch"        : "������˵Ŀ�ĵ������硰�찲�Ź㳡�������������硰���������������ź����硰����10086���������š�����������������Ӧ��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "VR_VDE_MAIN_MENU_WITHOUT_PHONE_CONNECTED_Info",
        "en"        : 'BT Phone is not connected. Please say "go to destination name" like "go to Tiananmen Square", or "News", "Weather".',
        "ch"        : "�����绰δ���ӣ�������˵Ŀ�ĵ������硰�찲�Ź㳡���������š�����������������Ӧ��",
        "fr"        : "",
    },
    
    
    
    {
        "start_id"  : -1,
        "name"      : "Setting_Audio_VR_voice_prompt",
        "en"        : "Voice Idication",
        "ch"        : "������ʾ",
        "fr"        : "",
    },
    
    
    #---------------------------------------------------------------------------------------------------------------
    
    
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_INVALID_COMMAND",
        "en"        : "Invalid input command",
        "ch"        : "�Ƿ�����������",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_NETWORK_ERROR",
        "en"        : "Network is not available, please try later",
        "ch"        : "���粻���ã����Ժ�����",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_NETWORK_SEARCHING",
        "en"        : "Searching, please wait...",
        "ch"        : "�������������Ժ�... ",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_NO_MATCH_KEYWORDS",
        "en"        : "No searching result, Please try later",
        "ch"        : "û����������������Ժ�����",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_NO_MATCH_RESULTS",
        "en"        : "No searching result, Please try later",
        "ch"        : "û����������������Ժ�����",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_ROUTE_CALCULATING",
        "en"        : "Calculating route",
        "ch"        : "���ڼ���·��",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_SET_DEST_FAILED",
        "en"        : "Setting Destination failure",
        "ch"        : "����·��ʧ��",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_ROUTE_CAL_FAILED",
        "en"        : "Route calculation failure",
        "ch"        : "·�߼���ʧ��",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_CLOUD_NOT_UNDERSTAND",
        "en"        : "Your input can not be undertood",
        "ch"        : "���������޷������",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_POI_INVALID_ITEM",
        "en"        : "The POI item is invalid",
        "ch"        : "��Ч��POI��Ŀ",
        "fr"        : "",
    },
    
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_NO_NETWORK_BUT_BT_PHONE_CONNECTED",
        "en"        : "Network is not available, only BT Phone VR is supported.",
        "ch"        : "���粻���ã���ǰ��֧�������绰����ʶ���ܡ�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_VR_VDE_NO_NETWORK_AND_NO_BT_PHONE_CONNECTED",
        "en"        : "Network is not available, BT Phone is not connected.",
        "ch"        : "���粻���ã������绰δ���ӣ�",
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
    #sys.stderr.write("%s%s" % (txtCmd, NEW_LINE));#########
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
    #print "����";
