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
        "ch"        : "您收到一个push object，%s: %s, 您需要打开吗？",
        "fr"        : "",
    },
    {
        "start_id"  : 38601,
        "name"      : "POPUP_NAV_PO_CALL_CONFIRMATION",
        "en"        : "Call: %s?",
        "ch"        : "拨打：%s？",
        "fr"        : "",
    },
    {
        "start_id"  : 38602,
        "name"      : "POPUP_NAV_PO_FAV_DUPLICATED",
        "en"        : "Item exists on Favorite lists.",
        "ch"        : "该条目在收藏夹中已经存在。",
        "fr"        : "",
    },
    {
        "start_id"  : 38603,
        "name"      : "POPUP_NAV_PO_FAV_FULL",
        "en"        : "Favorite List Full, clear list before trying again.",
        "ch"        : "收藏夹已满，请清除部分条目后再试。",
        "fr"        : "",
    },
    {
        "start_id"  : 38604,
        "name"      : "POPUP_NAV_PO_FAV_SAVING_SUCC",
        "en"        : "Saved to favorite.",
        "ch"        : "已保存到收藏夹中。",
        "fr"        : "",
    },
    {
        "start_id"  : 38605,
        "name"      : "POPUP_NAV_PO_AUTO_CALC_ROUTE_RUNNING",
        "en"        : "Calculating route…",
        "ch"        : "正在计算路线…",
        "fr"        : "",
    },
    
    {
        "start_id" : 1000,
    },
    {
        "start_id"  : -1,
        "name"      : "OPEN",
        "en"        : "Open",
        "ch"        : "打开",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "LATER",
        "en"        : "Later",
        "ch"        : "稍后",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "DISCARD",
        "en"        : "Discard",
        "ch"        : "丢弃",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "START",
        "en"        : "Start",
        "ch"        : "开始",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "SAVE_AS",
        "en"        : "Save as...",
        "ch"        : "保存...",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_PO_PG_MAIN_TITLE",
        "en"        : "Push Object - Guidance",
        "ch"        : "推送对象 - 向导",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_PO_FAV_FULL_VIEW_LIST",
        "en"        : "View List",
        "ch"        : "显示列表",
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
    #print "哈哈";
