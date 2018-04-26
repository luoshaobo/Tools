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

oDefaultPushGuidanceData = {
    "Protocol Version" : 1,                     # 0x01: 1 BYTE
    "VIN" : "1234567890",                       # 0x02: 10 BYTE
    "DSPT_Route_Info" : 0,                      # 0x03: 2 BYTE
    "Message unique ID" : -1,                   # 0x04: 2 BYTE
    "Service Type" : 13,                        # 0x05: 1 BYTE
    "Message Type" : 5,                         # 0x06: 1 BYTE
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
    "Description" : "",                         # 0x10: UTF8 STRING(30)
    "Destination Position LON" : 0,             # 0x13: LONG(4)
    "Destination Postion LAT" : 0,              # 0x14: LONG(4)
    "Stage 1 Position LON" : 0,                 # 0x15: LONG(4)
    "Stage 1 Position LAT" : 0,                 # 0x16: LONG(4)
    "Stage 2 Position LON" : 0,                 # 0x17: LONG(4)
    "Stage 2 Position LAT" : 0,                 # 0x18: LONG(4)
    "Stage 3 Position LON" : 0,                 # 0x19: LONG(4)
    "Stage 3 Position LAT" : 0,                 # 0x1A: LONG(4)
    "Stage 4 Position LON" : 0,                 # 0x1B: LONG(4)
    "Stage 4 Position LAT" : 0,                 # 0x1C: LONG(4)
    "GPS shifted Flag" : 0,                     # 0x20: 1 Byte
};

oPushGuidanceData = [
    {
        "__file_name__" : "PG_001_valid_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PG_001_valid_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "PG_002_valid_LongDescription_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC Dalian Road", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PG_002_valid_LongDescription_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC Dalian Road", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "PG_003_FourStages_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "Stage 1 Position LON" : 121499671,
        "Stage 1 Position LAT" : 31239878, 
        "Stage 2 Position LON" : 31239878,
        "Stage 2 Position LAT" : 9232993, 
        "Stage 3 Position LON" : 91492795,
        "Stage 3 Position LAT" : 31232993, 
        "Stage 4 Position LON" : 111492795,
        "Stage 4 Position LAT" : 8232993, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "PG_003_FourStages_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "Stage 1 Position LON" : 121499671,
        "Stage 1 Position LAT" : 31239878, 
        "Stage 2 Position LON" : 31239878,
        "Stage 2 Position LAT" : 9232993, 
        "Stage 3 Position LON" : 91492795,
        "Stage 3 Position LAT" : 31232993, 
        "Stage 4 Position LON" : 111492795,
        "Stage 4 Position LAT" : 8232993, 
        "GPS shifted Flag" : 0,
    },
    
    
    ##########################################################################################
    ## for Navi Push Object ITS
    ##
    {
        "__file_name__" : "Navigation_PushObj003_PG_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj003_PG_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj004_PG_InvalidGps_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 0,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj004_PG_InvalidGps_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 0,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj009_PG_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj009_PG_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj010_PG_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj010_PG_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj011_PG_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj011_PG_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj012_PG_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj012_PG_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Navigation_PushObj013_PG_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Navigation_PushObj013_PG_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    ##########################################################################################
    ## for Tele Push Object ITS
    ##
    {
        "__file_name__" : "Telematics_PushObj001_PG_001_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 1", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PG_001_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 1", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PG_002_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 2", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PG_002_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 2", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PG_003_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 3", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PG_003_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 3", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PG_004_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 4", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PG_004_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 4", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PG_005_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 5", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PG_005_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 5", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    
    {
        "__file_name__" : "Telematics_PushObj001_PG_006_A.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 6", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
        "GPS shifted Flag" : 0,
    },
    {
        "__file_name__" : "Telematics_PushObj001_PG_006_B.msg",
        "Message unique ID" : -1,
        "Description" : "KFC 6", 
        "Destination Position LON" : 121445299,
        "Destination Postion LAT" : 31223501, 
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
def CreatePushGuidanceMessage(oPushGuidanceDataItem):
    global nLastMessageID;
    
    sResult = "";
    
    ### 0x01: Protocol Version
    if oPushGuidanceDataItem.has_key("Protocol Version"):
        sResult01 = struct.pack(">B", oPushGuidanceDataItem["Protocol Version"]);
    else:
        sResult01 = struct.pack(">B", oDefaultPushGuidanceData["Protocol Version"]);
        
    ### 0x02: VIN
    if oPushGuidanceDataItem.has_key("VIN"):
        sResult02 = struct.pack(">10s", oPushGuidanceDataItem["VIN"]);
    else:
        sResult02 = struct.pack(">10s", oDefaultPushGuidanceData["VIN"]);
        
    ### 0x03: DSPT_Route_Info
    if oPushGuidanceDataItem.has_key("DSPT_Route_Info"):
        sResult03 = struct.pack(">H", oPushGuidanceDataItem["DSPT_Route_Info"]);
    else:
        sResult03 = struct.pack(">H", oDefaultPushGuidanceData["DSPT_Route_Info"]);
        
    ### 0x04: Message unique ID
    if oPushGuidanceDataItem.has_key("Message unique ID"):
        nMessageID = oPushGuidanceDataItem["Message unique ID"];
    else:
        nMessageID = oDefaultPushGuidanceData["Message unique ID"];
    if nMessageID == -1:
        nLastMessageID += 1;
        nMessageID = nLastMessageID;
    else:
        nLastMessageID = nMessageID;
    sResult04 = struct.pack(">H", nMessageID);
        
    ### 0x05: Service Type
    if oPushGuidanceDataItem.has_key("Service Type"):
        sResult05 = struct.pack(">B", oPushGuidanceDataItem["Service Type"]);
    else:
        sResult05 = struct.pack(">B", oDefaultPushGuidanceData["Service Type"]);
        
    ### 0x06: Message Type
    if oPushGuidanceDataItem.has_key("Message Type"):
        sResult06 = struct.pack(">B", oPushGuidanceDataItem["Message Type"]);
    else:
        sResult06 = struct.pack(">B", oDefaultPushGuidanceData["Message Type"]);
        
    ### 0x07: Time and date
    if oPushGuidanceDataItem.has_key("Time and date"):
        oTimeDate = oPushGuidanceDataItem["Time and date"];
    else:
        oTimeDate = oDefaultPushGuidanceData["Time and date"];
    nTimeDate = 0 \
        | (oTimeDate["Year"] << 26) \
        | (oTimeDate["Month"] << 22) \
        | (oTimeDate["Day"] << 17) \
        | (oTimeDate["Hour"] << 12) \
        | (oTimeDate["Minute"] << 6) \
        | (oTimeDate["Second"] << 0);
    sResult07 = struct.pack(">L", nTimeDate);
    
    ### 0x08: Message protocol version
    if oPushGuidanceDataItem.has_key("Message protocol version"):
        sResult08 = struct.pack(">B", oPushGuidanceDataItem["Message protocol version"]);
    else:
        sResult08 = struct.pack(">B", oDefaultPushGuidanceData["Message protocol version"]);
        
    ### 0x09: User Data Length
    # It will be processed later.
        
    ### 0x10: Description
    if oPushGuidanceDataItem.has_key("Description"):
        sDescription = oPushGuidanceDataItem["Description"];
    else:
        sDescription = oPushGuidanceDataItem["Description"];
    utf8Description = sDescription.decode("GBK").encode("UTF8");
    sResult10 = "";
    for ch in utf8Description:
        sResult10 += struct.pack(">B", ord(ch));
    
    ### 0x13: Destination Position LON
    if oPushGuidanceDataItem.has_key("Destination Position LON"):
        sResult13 = struct.pack(">L", oPushGuidanceDataItem["Destination Position LON"]);
    else:
        sResult13 = struct.pack(">L", oDefaultPushGuidanceData["Destination Position LON"]);
        
    ### 0x14: Destination Postion LAT
    if oPushGuidanceDataItem.has_key("Destination Postion LAT"):
        sResult14 = struct.pack(">L", oPushGuidanceDataItem["Destination Postion LAT"]);
    else:
        sResult14 = struct.pack(">L", oDefaultPushGuidanceData["Destination Postion LAT"]);
    
    ### 0x15: Stage 1 Position LON
    if oPushGuidanceDataItem.has_key("Stage 1 Position LON"):
        sResult15 = struct.pack(">L", oPushGuidanceDataItem["Stage 1 Position LON"]);
    else:
        sResult15 = struct.pack(">L", oDefaultPushGuidanceData["Stage 1 Position LON"]);
        
    ### 0x16: Stage 1 Position LAT
    if oPushGuidanceDataItem.has_key("Stage 1 Position LAT"):
        sResult16 = struct.pack(">L", oPushGuidanceDataItem["Stage 1 Position LAT"]);
    else:
        sResult16 = struct.pack(">L", oDefaultPushGuidanceData["Stage 1 Position LAT"]);
        
    ### 0x17: Stage 2 Position LON
    if oPushGuidanceDataItem.has_key("Stage 2 Position LON"):
        sResult17 = struct.pack(">L", oPushGuidanceDataItem["Stage 2 Position LON"]);
    else:
        sResult17 = struct.pack(">L", oDefaultPushGuidanceData["Stage 2 Position LON"]);
        
    ### 0x18: Stage 2 Position LAT
    if oPushGuidanceDataItem.has_key("Stage 2 Position LAT"):
        sResult18 = struct.pack(">L", oPushGuidanceDataItem["Stage 2 Position LAT"]);
    else:
        sResult18 = struct.pack(">L", oDefaultPushGuidanceData["Stage 2 Position LAT"]);
    
    ### 0x19: Stage 3 Position LON
    if oPushGuidanceDataItem.has_key("Stage 3 Position LON"):
        sResult19 = struct.pack(">L", oPushGuidanceDataItem["Stage 3 Position LON"]);
    else:
        sResult19 = struct.pack(">L", oDefaultPushGuidanceData["Stage 3 Position LON"]);
        
    ### 0x1A: Stage 3 Position LAT
    if oPushGuidanceDataItem.has_key("Stage 3 Position LAT"):
        sResult1A = struct.pack(">L", oPushGuidanceDataItem["Stage 3 Position LAT"]);
    else:
        sResult1A = struct.pack(">L", oDefaultPushGuidanceData["Stage 3 Position LAT"]);
        
    ### 0x1B: Stage 4 Position LON
    if oPushGuidanceDataItem.has_key("Stage 4 Position LON"):
        sResult1B = struct.pack(">L", oPushGuidanceDataItem["Stage 4 Position LON"]);
    else:
        sResult1B = struct.pack(">L", oDefaultPushGuidanceData["Stage 4 Position LON"]);
    
    ### 0x1C: Stage 4 Position LAT
    if oPushGuidanceDataItem.has_key("Stage 4 Position LAT"):
        sResult1C = struct.pack(">L", oPushGuidanceDataItem["Stage 4 Position LAT"]);
    else:
        sResult1C = struct.pack(">L", oDefaultPushGuidanceData["Stage 4 Position LAT"]);
    
    ### 0x20: GPS shifted Flag
    if oPushGuidanceDataItem.has_key("GPS shifted Flag"):
        sResult20 = struct.pack(">B", oPushGuidanceDataItem["GPS shifted Flag"]);
    else:
        sResult20 = struct.pack(">B", oDefaultPushGuidanceData["GPS shifted Flag"]);
    
    ######
    sResultMessageData = "\x7E\x10%s\x7E\x13%s\x7E\x14%s\x7E\x15%s\x7E\x16%s\x7E\x17%s\x7E\x18%s\x7E\x19%s\x7E\x1A%s\x7E\x1B%s\x7E\x1C%s\x7E\x20%s" % \
        (sResult10, sResult13, sResult14, sResult15, sResult16, sResult17, sResult18, sResult19, sResult1A, sResult1B, sResult1C, sResult20);
    nMessageDataLen = len(sResultMessageData);
    sResult09 = struct.pack(">3B", 
        ((nMessageDataLen >> 16) & 0xFF),
        ((nMessageDataLen >>  8) & 0xFF),
        ((nMessageDataLen >>  0) & 0xFF)
    );
    sResultMessageHeader = "\x01%s\x7E\x02%s\x7E\x03%s\x7E\x04%s\x7E\x05%s\x7E\x06%s\x7E\x07%s\x7E\x08%s\x7E\x09%s" % (
        sResult01, sResult02, sResult03, sResult04, sResult05, sResult06, sResult07, sResult08, sResult09);
    sResult = sResultMessageHeader + sResultMessageData;
        
    write_str_to_file(sResult, RESULT_BAST_DIR + "\\" + oPushGuidanceDataItem["__file_name__"]);
    
    return True;

def CreatePushGuidanceMessages():
    for oPushGuidanceDataItem in oPushGuidanceData:
        if not CreatePushGuidanceMessage(oPushGuidanceDataItem):
            LOG_ERR("*** ERROR: CreatePushGuidanceMessages(): CreatePushGuidanceMessage() failed!%s" % (NEW_LINE));
            exit(-1);


def main():  
    CreatePushGuidanceMessages();

    return 0;

if __name__ == "__main__":
    main();