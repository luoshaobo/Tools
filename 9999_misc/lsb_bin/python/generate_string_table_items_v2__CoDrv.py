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
        "start_id" : 1700,
    },
    
    #-----------------------------------------------------------------------------------------------
    # ͨ��
    #
    {
        "start_id"  : -1,
        "name"      : "REALTIME_INFO_PART1",
        "en"        : "Real-",
        "ch"        : "ʵʱ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "REALTIME_INFO_PART2",
        "en"        : "time",
        "ch"        : "��Ϣ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_CO_DRIVING",
        "en"        : "Co-Driving",
        "ch"        : "��ͬ��ʻ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_CO_DRIVING_PART1",
        "en"        : "Co-",
        "ch"        : "��ͬ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_CO_DRIVING_PART2",
        "en"        : "Driving",
        "ch"        : "��ʻ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_EDIT_STAGES",
        "en"        : "Edit stages",
        "ch"        : "�༭;����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_EDIT_STAGES_PART1",
        "en"        : "Edit",
        "ch"        : "�༭",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_EDIT_STAGES_PART2",
        "en"        : "stages",
        "ch"        : ";����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_PICKUP",
        "en"        : "Pick up",
        "ch"        : "����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_ADD_TO_STAGE",
        "en"        : "Set as stage",
        "ch"        : "����Ϊ;����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_ADD_TO_STAGE_PART1",
        "en"        : "Set as",
        "ch"        : "����Ϊ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_ADD_TO_STAGE_PART2",
        "en"        : "stage",
        "ch"        : ";����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_QR_CODE",
        "en"        : "QR Code",
        "ch"        : "��ά��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_QR_CODE_PART1",
        "en"        : "QR",
        "ch"        : "��ά",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_QR_CODE_PART2",
        "en"        : "Code",
        "ch"        : "��",
        "fr"        : "",
    },
    #-----------------------------------------------------------------------------------------------
    # NavMap������
    #
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_NAV_MAP_RECORD_BEGIN_INDICATION_1",
        "en"        : "Recording voice",
        "ch"        : "����¼��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_NAV_MAP_RECORD_BEGIN_INDICATION_2",
        "en"        : "Press again the voice button",
        "ch"        : "�ٴΰ���������ť",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_NAV_MAP_RECORD_BEGIN_INDICATION_3",
        "en"        : "to send voice",
        "ch"        : "��ʼ����",
        "fr"        : "",
    },
    #-----------------------------------------------------------------------------------------------
    # ɨ���ά�����
    #
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SETTING_SCAN_QR_CODE",
        "en"        : "Co-Driving QR Code",
        "ch"        : "��ͬ��ʻ��ά��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_INIT_QR_CODE_DEVICE_ID",
        "en"        : "Device ID: ",
        "ch"        : "�豸�ţ� ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_INIT_QR_CODE_TITLE",
        "en"        : "Co-Driving QR Code",
        "ch"        : "��ͬ��ʻ��ά��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_INIT_QR_CODE_INSTRUCTION_PART1",
        "en"        : "Please scan above QR code, and follow",
        "ch"        : "��ɨ������Ķ�ά�룬������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_INIT_QR_CODE_INSTRUCTION_PART2",
        "en"        : "the detail use instruction on phone.",
        "ch"        : "�ֻ��ϵ���ϸʹ��˵�����в�����",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_UNI_QR_CODE_TITLE",
        "en"        : "Co-Driving QR Code",
        "ch"        : "��ͬ��ʻ��ά��",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_UNI_QR_CODE_INSTRUCTION_PART1",
        "en"        : "Please scan above QR code, and follow",
        "ch"        : "��ɨ������Ķ�ά�룬������",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_SCAN_UNI_QR_CODE_INSTRUCTION_PART2",
        "en"        : "the detail use instruction on phone.",
        "ch"        : "�ֻ��ϵ���ϸʹ��˵�����в�����",
        "fr"        : "",
    },
    #-----------------------------------------------------------------------------------------------
    # NavDestSummary������
    #
    {
        "start_id"  : -1,
        "name"      : "NAV_DEST_SUMMARY_TRIP_TITLE",
        "en"        : "Trip title: ",
        "ch"        : "�г̱���: ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_DEST_SUMMARY_TRIP_DEST",
        "en"        : "Trip destination: ",
        "ch"        : "�г�Ŀ�ĵ�: ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_DEST_SUMMARY_TRIP_MEMBER_COUNT",
        "en"        : "Trip active members: ",
        "ch"        : "�г̻��Ա: ",
        "fr"        : "",
    },
    {
        "start_id"  : -1,
        "name"      : "NAV_DEST_SUMMARY_TRIP_VALID_DATA_TIME",
        "en"        : "Trip valid data/time: ",
        "ch"        : "�г���Ч����/ʱ��: ",
        "fr"        : "",
    },
    #-----------------------------------------------------------------------------------------------
    # TripMemberList�����У�
    #
    {
        "start_id"  : -1,
        "name"      : "NAV_CODRV_TRIP_MEMBER_LIST_TITLE",
        "en"        : "Trip member list",
        "ch"        : "�г̳�Ա�б�",
        "fr"        : "",
    },
    
    

    
    #-----------------------------------------------------------------------------------------------
    # �����⻭����
    #
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_CODRV_NEW_ACTIVE_TRIP",
        "en"        : "%s has sent a trip request. The trip destination is %s. Do you want to start the trip guidance?",
        "ch"        : "%s�������г������г�Ŀ�ĵ���%s����ȷ����ʼ���г���",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_CODRV_NEW_ACTIVE_TRIP_TYPE2",
        "en"        : "%s has sent a trip request. The trip destination is %s. Do you want to replace the current guidance?",
        "ch"        : "%s�������г������г�Ŀ�ĵ���%s����ȷ��ȡ����ǰ������",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_CODRV_NEW_ACTIVE_TRIP_TYPE3",
        "en"        : "Trip leader %s has changed the destination to %s. Do you want to recalculate now?",
        "ch"        : "�г��쵼��%s�Ѿ���Ŀ�ĵظ�Ϊ%s����ȷ�����¼���·����",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_CODRV_SOMEONE_REQUEST_TO_BE_PICKED_UP",
        "en"        : "%s sends you a pickup request. The address is %s.",
        "ch"        : "%s�������ʹ������� ��ַ��%s��",
        "fr"        : "",
    },
    #-----------------------------------------------------------------------------------------------
    # TripMemberList�����У�
    #
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_CODRV_TOO_MANY_STAGES_WITHOUT_GUIDANCE",
        "en"        : "The maximum number of stages is 5.",
        "ch"        : "���;������ĿΪ5��",
        "fr"        : "",
    },
    {
        "start_id"  : -2,
        "name"      : "POPUP_NAV_CODRV_TOO_MANY_STAGES_WITH_GUIDANCE",
        "en"        : "The maximum number of stages is 5. Currently you can only pick up %d members!",
        "ch"        : "���;������ĿΪ5�� Ŀǰ��ֻ���ܴ���%d����Ա��",
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
