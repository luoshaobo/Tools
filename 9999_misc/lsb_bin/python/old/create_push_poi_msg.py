#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# NOTE:
#     please execute this program in Chinese environment (by using AppLocale: Simple Chinese)

import sys;
import os;
import string;
import xml.dom.minidom;
import struct;

RESULT_BAST_DIR = ".\\temp\\PO";

NEW_LINE = "\r\n"

oDefaultPushPOIData = {
    "Protocol Version" : 1,                     # 0x01: 1 BYTE
    "VIN" : "1234567890",                       # 0x02: 10 BYTE
    "DSPT_Route_Info" : 0,                      # 0x03: 2 BYTE
    "Message unique ID" : -1,                   # 0x04: 2 BYTE
    "Service Type" : 13,                        # 0x05: 1 BYTE
    "Message Type" : 4,                         # 0x06: 1 BYTE
    "Time and date" :                           # 0x07: 
    {
        "Year" : 0,                             #     6 bits: 0 = 2000 ; 1 = 2001 ; 2 = 2002; ...
        "Month" : 1,                            #     4 bits: 1 ~ 12
        "Day" : 2,                              #     5 bits: 1 ~ 31
        "Hour" : 3,                             #     5 bits: 0 ~ 23
        "Minute" : 4,                           #     6 bits: 0 ~ 59
        "Second" : 5,                           #     6 bits: 0 ~ 59
    },
    "Message protocol version" : 0,             # 0x08: 1BYTE
    "User Data Length" : 0,                     # 0x09: 3 BYTE
    "NAME" : "",                                # 0x11: UTF8 STRING(60) 
    "LON" : 0,                                  # 0x13: LONG(4)
    "LAT" : 0,                                  # 0x14: LONG(4)
    "TEL" : "",                                 # 0x15: STRING(16)
    "GPS shifted Flag" : 0,                     # 0x20: 1 Byte
};

oPushPOIData = [
    {
        "__file_name__" : "PP_001_valid_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PP_001_valid_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "PP_002_valid_LongPhoneNo_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 1", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "1234567890123456",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PP_002_valid_LongPhoneNo_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 1", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "1234567890123456",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "PP_003_valid_LongName_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC (Dalian Road)", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PP_003_valid_LongName_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC (Dalian Road)", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "PP_004_valid_LongPhoneNoPlus_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        #"TEL" : "+8602198976855",
        #"TEL" : "+8613661412448",
        "TEL" : "+862160804724",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PP_004_valid_LongPhoneNoPlus_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        #"TEL" : "+8602198976855",
        #"TEL" : "+8613661412448",
        "TEL" : "+862160804724",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "PP_005_valid_beiwaitan_A.msg",
        "Message unique ID" : -1,
        "NAME" : "北外滩东信酒店停车场", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PP_005_valid_beiwaitan_B.msg",
        "Message unique ID" : -1,
        "NAME" : "北外滩东信酒店停车场", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    ##########################################################################################
    ## for Navi Push Object ITS
    ##
    {
        "__file_name__" : "Navigation_PushObj001_PP_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj001_PP_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj002_PP_InvalidGps_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 0,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj002_PP_InvalidGps_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 0,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj005_PP_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj005_PP_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj006_PP_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj006_PP_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj007_PP_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj007_PP_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj008_PP_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj008_PP_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    ##########################################################################################
    ## for Tele Push Object ITS
    ##
    {
        "__file_name__" : "Telematics_PushObj001_PP_001_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 1", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_001_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 1", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_002_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 2", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_002_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 2", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_003_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 3", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_003_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 3", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_004_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 4", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_004_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 4", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_005_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 5", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_005_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 5", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_006_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 6", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_006_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 6", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_007_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 7", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_007_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 7", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_008_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 8", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_008_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 8", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PP_009_A.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 9", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PP_009_B.msg",
        "Message unique ID" : -1,
        "NAME" : "KFC 9", 
        "LON" : 121445299,
        "LAT" : 31223501, 
        "TEL" : "02198976855",
        "GPS shifted Flag" : 0,
    },
];

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def write_str_to_file(strContent, strFilePath):
    oFile = file(strFilePath, "w+b");
    if oFile != None:
        oFile.write(strContent);
    oFile = None;

nLastMessageID = 0;
def CreatePushPOIMessage(oPushPOIDataItem):
    global nLastMessageID;
    
    sResult = "";
    
    ### 0x01: Protocol Version
    if oPushPOIDataItem.has_key("Protocol Version"):
        sResult01 = struct.pack(">B", oPushPOIDataItem["Protocol Version"]);
    else:
        sResult01 = struct.pack(">B", oDefaultPushPOIData["Protocol Version"]);
        
    ### 0x02: VIN
    if oPushPOIDataItem.has_key("VIN"):
        sResult02 = struct.pack(">10s", oPushPOIDataItem["VIN"]);
    else:
        sResult02 = struct.pack(">10s", oDefaultPushPOIData["VIN"]);
        
    ### 0x03: DSPT_Route_Info
    if oPushPOIDataItem.has_key("DSPT_Route_Info"):
        sResult03 = struct.pack(">H", oPushPOIDataItem["DSPT_Route_Info"]);
    else:
        sResult03 = struct.pack(">H", oDefaultPushPOIData["DSPT_Route_Info"]);
        
    ### 0x04: Message unique ID
    if oPushPOIDataItem.has_key("Message unique ID"):
        nMessageID = oPushPOIDataItem["Message unique ID"];
    else:
        nMessageID = oDefaultPushPOIData["Message unique ID"];
    if nMessageID == -1:
        nLastMessageID += 1;
        nMessageID = nLastMessageID;
    else:
        nLastMessageID = nMessageID;
    sResult04 = struct.pack(">H", nMessageID);
        
    ### 0x05: Service Type
    if oPushPOIDataItem.has_key("Service Type"):
        sResult05 = struct.pack(">B", oPushPOIDataItem["Service Type"]);
    else:
        sResult05 = struct.pack(">B", oDefaultPushPOIData["Service Type"]);
        
    ### 0x06: Message Type
    if oPushPOIDataItem.has_key("Message Type"):
        sResult06 = struct.pack(">B", oPushPOIDataItem["Message Type"]);
    else:
        sResult06 = struct.pack(">B", oDefaultPushPOIData["Message Type"]);
        
    ### 0x07: Time and date
    if oPushPOIDataItem.has_key("Time and date"):
        oTimeDate = oPushPOIDataItem["Time and date"];
    else:
        oTimeDate = oDefaultPushPOIData["Time and date"];
    nTimeDate = 0 \
        | (oTimeDate["Year"] << 26) \
        | (oTimeDate["Month"] << 22) \
        | (oTimeDate["Day"] << 17) \
        | (oTimeDate["Hour"] << 12) \
        | (oTimeDate["Minute"] << 6) \
        | (oTimeDate["Second"] << 0);
    sResult07 = struct.pack(">L", nTimeDate);
    
    ### 0x08: Message protocol version
    if oPushPOIDataItem.has_key("Message protocol version"):
        sResult08 = struct.pack(">B", oPushPOIDataItem["Message protocol version"]);
    else:
        sResult08 = struct.pack(">B", oDefaultPushPOIData["Message protocol version"]);
        
    ### 0x09: User Data Length
    # It will be processed later.
        
    ### 0x11: NAME
    if oPushPOIDataItem.has_key("NAME"):
        sName = oPushPOIDataItem["NAME"];
    else:
        sName = oPushPOIDataItem["NAME"];
    utf8Name = sName.decode("GBK").encode("UTF8");
    sResult11 = "";
    for ch in utf8Name:
        sResult11 += struct.pack(">B", ord(ch));
    
    ### 0x13: LON
    if oPushPOIDataItem.has_key("LON"):
        sResult13 = struct.pack(">L", oPushPOIDataItem["LON"]);
    else:
        sResult13 = struct.pack(">L", oDefaultPushPOIData["LON"]);
        
    ### 0x14: LAT
    if oPushPOIDataItem.has_key("LAT"):
        sResult14 = struct.pack(">L", oPushPOIDataItem["LAT"]);
    else:
        sResult14 = struct.pack(">L", oDefaultPushPOIData["LAT"]);
        
    ### 0x15: TEL
    if oPushPOIDataItem.has_key("TEL"):
        sTel = oPushPOIDataItem["TEL"];
    else:
        sTel = oDefaultPushPOIData["TEL"];
    sResult15 = "";
    i = 0;
    while i < len(sTel) and i < 32:
        sCh = sTel[i];
        if sCh == "+":
            nCh = 0x0C;
        elif ord("0") <= ord(sCh) and ord(sCh) <= ord("9"):
            nCh = ord(sCh) - ord("0");
        else:
            nCh = 0;
            LOG_ERR("*** ERROR: telephone number is out of range!");
            exit(-1);
        if i % 2 == 0:
            nCh2 = nCh << 4;
        else:
            nCh2 += nCh;
            sResult15 += struct.pack(">B", nCh2);
        i += 1;
    while i < 32:
        nCh = 0x0F;
        if i % 2 == 0:
            nCh2 = nCh << 4;
        else:
            nCh2 += nCh;
            sResult15 += struct.pack(">B", nCh2);
        i += 1;
        
    ### 0x20: GPS shifted Flag
    if oPushPOIDataItem.has_key("GPS shifted Flag"):
        sResult20 = struct.pack(">B", oPushPOIDataItem["GPS shifted Flag"]);
    else:
        sResult20 = struct.pack(">B", oDefaultPushPOIData["GPS shifted Flag"]);
    
    ######
    sResultMessageData = "\x7E\x11%s\x7E\x13%s\x7E\x14%s\x7E\x15%s\x7E\x20%s" % (sResult11, sResult13, sResult14, sResult15, sResult20);
    nMessageDataLen = len(sResultMessageData);
    sResult09 = struct.pack(">3B", 
        ((nMessageDataLen >> 16) & 0xFF),
        ((nMessageDataLen >>  8) & 0xFF),
        ((nMessageDataLen >>  0) & 0xFF)
    );
    sResultMessageHeader = "\x01%s\x7E\x02%s\x7E\x03%s\x7E\x04%s\x7E\x05%s\x7E\x06%s\x7E\x07%s\x7E\x08%s\x7E\x09%s" % (
        sResult01, sResult02, sResult03, sResult04, sResult05, sResult06, sResult07, sResult08, sResult09);
    sResult = sResultMessageHeader + sResultMessageData;
        
    write_str_to_file(sResult, RESULT_BAST_DIR + "\\" + oPushPOIDataItem["__file_name__"]);
    
    return True;

def CreatePushPOIMessages():
    for oPushPOIDataItem in oPushPOIData:
        if not CreatePushPOIMessage(oPushPOIDataItem):
            LOG_ERR("*** ERROR: CreatePushPOIMessages(): CreatePushPOIMessage() failed!%s" % (NEW_LINE));
            exit(-1);


def main():  
    CreatePushPOIMessages();

    return 0;

if __name__ == "__main__":
    main();