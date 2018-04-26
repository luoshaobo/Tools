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
        "en"        : "POI",
        "ch"        : "��Ȥ��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "GUIDANCE",
        "en"        : "Guidance",
        "ch"        : "����",
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
        "ch"        : "���Ͷ��� - ����",
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
        "en"        : "Auto routing for Push Object",
        "ch"        : "���Ͷ����Զ�·������",
        "fr"        : "",
    },
    
    
    
    
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_CALL_CONFIRMATION",
        "en"        : "Call: %s?",
        "ch"        : "����%s��",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_FAV_DUPLICATED",
        "en"        : "Item exists on Favorite lists.",
        "ch"        : "����Ŀ���ղؼ����Ѿ����ڡ�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_FAV_FULL",
        "en"        : "Favorite List Full, clear list before trying again.",
        "ch"        : "�ղؼ������������������Ŀ�����ԡ�",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_PO_FAV_SAVING_SUCC",
        "en"        : "Saved to favorite.",
        "ch"        : "�ѱ��浽�ղؼ��С�",
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
        "en"        : "You have received a push object, %s: %s, do you wish to open?",
        "ch"        : "���յ�һ�����Ͷ���%s: %s, ����Ҫ����",
        "fr"        : "",
    },
]

####################################################################################################
## common
##

FILE_PATH__SCREENID_VTYP                                = """D:\casdev\my_code\C1.B5.02_PushObject\HMI\Infrastructure\DataTypes\ScreenID.vtyp"""
FILE_PATH__LOCALIZEDTEXTTABLE_VTBL                      = """D:\casdev\my_code\C1.B5.02_PushObject\HMI\Tables\Localization\LocalizedTextTable.vtbl"""

SHELL_CMD_TEMPLATE__FIND_SCREEN_ID                      = """grep "%s" '%s' | sed 's/^[ \t]*<named_value name="[^"]\+"[ \t]\+value="\([0-9]\+\)"[ \t]*\/>[ \t]*$/\\1/g'"""
SHELL_CMD_TEMPLATE__FIND_STR_UUID                       = """grep "%s" '%s' | sed 's/^[ \t]*<object name="[^"]\+" class="LocalizedTextTableElement" guid="\([^"]\+\)"[ \t]*>$/\\1/g'"""

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
    #print "����";
