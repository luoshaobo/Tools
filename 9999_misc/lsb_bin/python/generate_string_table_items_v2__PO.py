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
        "start_id" : 1000,
    },
    {
        "start_id"  : -1,
        "name"      : "POI",
        "en"        : "point of interest information",
        "ch"        : "��Ȥ����Ϣ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "GUIDANCE",
        "en"        : "guidance information",
        "ch"        : "·������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "OPEN",
        "en"        : "Open",
        "ch"        : "��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "LATER",
        "en"        : "Later",
        "ch"        : "�Ժ�",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "DISCARD",
        "en"        : "Discard",
        "ch"        : "����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "START",
        "en"        : "Start",
        "ch"        : "��ʼ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "SAVE_AS",
        "en"        : "Save as",
        "ch"        : "����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_PO_PG_MAIN_TITLE",
        "en"        : "Push Object - Guidance",
        "ch"        : "���Ͷ��� - ·������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_PO_FAV_FULL_VIEW_LIST",
        "en"        : "View List",
        "ch"        : "��ʾ�б�",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "AUTO_ROUTING_FOR_PUSH_OBJECT",
        "en"        : "Auto Routing for Push Guidance",
        #"ch"        : "���Ͷ����Զ�·�߼���",
        "ch"        : "�Զ��������͵���·��",
        "fr"        : "",
    },
    
    
    
    
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_CALL_CONFIRMATION",
        "en"        : "Call: %s?",
        "ch"        : "���У�%s��",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_FAV_DUPLICATED",
        "en"        : "Item exists on Favorite lists.",
        "ch"        : "��Ŀ�Ѵ������ղ��б�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_FAV_FULL",
        "en"        : "Favorite list is full. Please delete an existing favorite before trying to save again.",
        "ch"        : "�ղؼ���������Ҫ�����µ��ղأ�����ɾ��һ���ɵ��ղء�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_FAV_SAVING_SUCC",
        "en"        : "Favorite Saved",
        #"ch"        : "�ѱ��浽�ղ��б�",
        "ch"        : "���ղ�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_AUTO_CALC_ROUTE_RUNNING",
        "en"        : "Calculating route��",
        "ch"        : "���ڼ���·�ߡ�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_OPEN_INDICATION",
        "en"        : "You have received a %s:\\X000D\\X000A%s\\X000D\\X000A \\X000D\\X000ADo you wish to open?",
        "ch"        : "�����յ�����%s:\\X000D\\X000A%s\\X000D\\X000A \\X000D\\X000A�Ƿ����ڴ򿪣�",
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
