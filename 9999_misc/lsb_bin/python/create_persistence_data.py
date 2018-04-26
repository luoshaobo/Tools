#!/bin/python

#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# NOTE:
#     please execute this program in Chinese environment (by using AppLocale: Simple Chinese)

import sys;
import os;
import string;
import xml.dom.minidom;
import struct;

NEW_LINE = "\r\n";

def Output(strMsg):
    sys.stdout.write(strMsg);

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def write_str_to_file(strContent, strFilePath):
    oFile = file(strFilePath, "w+b");
    if oFile != None:
        oFile.write(strContent);
    oFile = None;
    
def usage():
    print "Usage:";
    print "    %s [tpeg]" % (sys.argv[0]);

def process_tpeg():
    """
    typedef struct{
        bool_t trafficInfoStatus;
        bool_t trafficInfoOverFM;
        uint16_t trafficPeriod;
        uc8_t trafficURL[MAX_CFG_URL_STRING_LENGTH]; // #define MAX_CFG_URL_STRING_LENGTH       100
    }S_TLM_REMOTE_CFG_TRAFFIC;
    """
    oTpegConfig = {
        "trafficInfoStatus" : 1,
        "trafficInfoOverFM" : 0,
        "trafficPeriod"     : 300,
        "trafficURL"        : "192.168.1.25:20330",
    };
    sResult = "";
    sResult = struct.pack("<BBH100s",
        oTpegConfig["trafficInfoStatus"],
        oTpegConfig["trafficInfoOverFM"],
        oTpegConfig["trafficPeriod"],
        oTpegConfig["trafficURL"].decode("GBK").encode("UTF8")
    );
    for ch in sResult:
        Output("%02X " % (ord(ch)));
    print "";
    
def main():
    if len(sys.argv) == 1:
        usage();
    elif len(sys.argv) >= 2:
        if sys.argv[1] == "--help" or sys.argv[1] == "-H":
            usage();
            
    if len(sys.argv) >= 2:
        if sys.argv[1] == "tpeg":
            process_tpeg();

    return 0;

if __name__ == "__main__":
    main();