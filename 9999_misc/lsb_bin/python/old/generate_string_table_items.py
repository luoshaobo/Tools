#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# NOTE:
#     please execute this program in Chinese environment (by using AppLocale: Simple Chinese)

import sys
import os

NEW_LINE = "\r\n"

oLocalTextItemList = [
    {
        "start_id"  : 38600,
        "name"      : "POPUP_NAV_PO_OPEN_INDICATION",
        "en"        : "You have received a push object, %s: %s, do you wish to open?",
        "ch"        : "���յ�һ��push object��%s: %s, ����Ҫ����",
        "fr"        : "",
    },
    {
        "start_id"  : 38601,
        "name"      : "POPUP_NAV_PO_CALL_CONFIRMATION",
        "en"        : "Call: %s?",
        "ch"        : "����%s��",
        "fr"        : "",
    },
    {
        "start_id"  : 38602,
        "name"      : "POPUP_NAV_PO_FAV_DUPLICATED",
        "en"        : "Item exists on Favorite lists.",
        "ch"        : "����Ŀ���ղؼ����Ѿ����ڡ�",
        "fr"        : "",
    },
    {
        "start_id"  : 38603,
        "name"      : "POPUP_NAV_PO_FAV_FULL",
        "en"        : "Favorite List Full, clear list before trying again.",
        "ch"        : "�ղؼ������������������Ŀ�����ԡ�",
        "fr"        : "",
    },
    {
        "start_id"  : 38604,
        "name"      : "POPUP_NAV_PO_FAV_SAVING_SUCC",
        "en"        : "Saved to favorite.",
        "ch"        : "�ѱ��浽�ղؼ��С�",
        "fr"        : "",
    },
    {
        "start_id"  : 38605,
        "name"      : "POPUP_NAV_PO_AUTO_CALC_ROUTE_RUNNING",
        "en"        : "Calculating route��",
        "ch"        : "���ڼ���·�ߡ�",
        "fr"        : "",
    },
    
    {
        "start_id" : 1000,
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
        "en"        : "Save as...",
        "ch"        : "����...",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_PO_PG_MAIN_TITLE",
        "en"        : "Push Object - Guidance",
        "ch"        : "���Ͷ��� - ��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_PO_FAV_FULL_VIEW_LIST",
        "en"        : "View List",
        "ch"        : "��ʾ�б�",
        "fr"        : "",
    },
]

####################################################################################################
## common
##
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

def main():
    sResultStr = "";
    nStartId = 0;
    for oLocalTextItem in oLocalTextItemList:
        if oLocalTextItem["start_id"] != -1:
            nStartId = oLocalTextItem["start_id"] - 1;
            if not oLocalTextItem.has_key("name"):
                continue;
            else:
                nStartId += 1;
        else:
            nStartId += 1;
        sResultStr += "  <object name=\"%s\" class=\"LocalizedTextTableElement\" guid=\"%s\">%s" % (oLocalTextItem["name"], gen_uuid(), NEW_LINE);
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
