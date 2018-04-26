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

DEF_OUTPUT_FILE_PATH = "./unicode_output.txt";

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def write_str_to_file(strContent, strFilePath):
    oFile = file(strFilePath, "w+b");
    if oFile != None:
        oFile.write(strContent);
    oFile = None;
    
def convert_on_char(sCharStr):
    sResult = struct.pack("<H", string.atoi(sCharStr, 10));
    return sResult;
    
def process():
    sResult = "\xFF\xFE";
    
    if len(sys.argv) < 2:
        LOG_ERR("*** ERROR: no input file is given!%s" % (NEW_LINE));
        return -1;
    
    oFile = file(sys.argv[1], "rb");
    if (oFile == None):
        LOG_ERR("*** ERROR: the input file can't be opened!%s" % (NEW_LINE));
        return -1;
    
    oLines = oFile.readlines();
    
    for sLine in oLines:
        sLine = sLine.strip();
        if sLine != None and sLine != "":
            sResult = sResult + convert_on_char(sLine);
    
    sOutputFilePath = DEF_OUTPUT_FILE_PATH;
    if len(sys.argv) >= 3:
        sOutputFilePath = sys.argv[2];
    
    write_str_to_file(sResult, sOutputFilePath);

def main():  
    process();

    return 0;

if __name__ == "__main__":
    main();