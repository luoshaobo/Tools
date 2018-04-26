#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# NOTE:
#     please execute this program in Chinese environment (by using AppLocale: Simple Chinese)

import sys;
import os;
import string;
import xml.dom.minidom;

NEW_LINE = "\r\n"

FILE_PATH__LOCALIZEDTEXTTABLE_VTBL                      = """D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\HMI\\Tables\\Localization\\LocalizedTextTable.vtbl""";
FILE_PATH__LOCALIZEDTEXTTABLE_VTBL_2                    = """temp\\xxx.xml""";

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def PrintToStdOut(strMsg):
    sys.stdout.write(strMsg);
    
def get_list_from_shell_command(strShellCommand):
    oList = [];
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

#
# NOTE: only the first node with strNodeName be processed.
#
def GetXmlNoteAttribVal(strXmlContent, strNodeName, strAttribName):
    #LOG_INF("### INFO: %s%s%s" % (NEW_LINE, strXmlContent, NEW_LINE));

    oDom = xml.dom.minidom.parseString(strXmlContent);
    if oDom == None:
        return None;
    oNodes = oDom.getElementsByTagName(strNodeName);
    if oNodes == None or oNodes[0] == None:
        LOG_INF("### INFO: Can't find xml node: %s%s" % (strNodeName, NEW_LINE));
        return "";
    
    if strAttribName != None and strAttribName != "":
        oAttribs = oNodes[0].attributes;
        if oAttribs == None or oAttribs[strAttribName] == None:
            LOG_INF("### INFO: Can't find xml node/attrib: %s/%s%s" % (strNodeName, strAttribName, NEW_LINE));
            return "";
        return oAttribs[strAttribName].value;
    else:
        oNodes = oNodes[0].childNodes;
        if oNodes == None:
            LOG_INF("### INFO: the xml node has no TEXT_NODE: %s%s" % (strNodeName, NEW_LINE));
            return "";
        for oNode in oNodes:
            if oNode.nodeType == oNode.TEXT_NODE:
                return oNode.data;
        LOG_INF("### INFO: the xml node has no TEXT_NODE: %s%s" % (strNodeName, NEW_LINE));
        return "";

def GetRecordStr(oFile):
    strResult = "";
    
    if oFile == None:
        return None;
        
    bBegin = False;
    bEnd = False;
    while True:
        strLine = oFile.readline();
        if strLine == "":   # EOF
            return "";
        strStripedLine = strLine.strip();
        if strStripedLine[0:7] == "<object":
            bBegin = True;
        if strStripedLine[0:9] == "</object>":
            bEnd = True;
        if bBegin:
            strResult += strLine;
        if bEnd:
            break;
            
    return strResult;

def ParseFileToDict(strFilePath, strEncode):
    oResultDic = {};

    oFile = file(strFilePath, "r");
    if oFile == None:
        LOG_ERR("*** ERROR: can't open file to read: %s%s" % (strFilePath, NEW_LINE));
        exit(-1);
    while True:
        strRecord = GetRecordStr(oFile);
        if strRecord == None or strRecord == "":
            break;
        strRecord = strRecord.decode(strEncode).encode("UTF-8");
        strObjectName = GetXmlNoteAttribVal(strRecord, "object", "name");
        if strObjectName == None or strObjectName == "":
            LOG_ERR("*** ERROR: Calling GetXmlNoteAttribVal() to get strObjectName is failed!%s" % (NEW_LINE));
            exit(-1);
        strProp = GetXmlNoteAttribVal(strRecord, "prop", "");
        if strProp == None or strProp == "":
            LOG_ERR("*** ERROR: Calling GetXmlNoteAttribVal() to get strProp is failed!%s" % (NEW_LINE));
            exit(-1);
        #LOG_INF("### INFO: %s:%s%s" % (strObjectName, strProp, NEW_LINE));
        oResultDic[strObjectName] = (strProp, strRecord);
    
    return oResultDic;
    
def OutputHeader():
    strHeader = """<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE table PUBLIC "-//PresagisCanada//DTD VapsXT 14.0" "file:///C:/VAPSXTDTD/VapsXT_14.dtd">

<table name="LocalizedTextTable" type="LocalizedTextTable">
  <model>
    <prop name="DefaultValue">Default</prop>
  </model>
""";
    PrintToStdOut(strHeader);
    
def OutputTailer():
    strTailer = """</table>
""";
    PrintToStdOut(strTailer);

def main():  
    oResultDic1 = ParseFileToDict(FILE_PATH__LOCALIZEDTEXTTABLE_VTBL, "UTF-8");
    oResultDic2 = ParseFileToDict(FILE_PATH__LOCALIZEDTEXTTABLE_VTBL_2, "GBK");
    
    for oKey in oResultDic2.iterkeys():
        if oResultDic1.has_key(oKey):
            del oResultDic1[oKey];
        
    oResultDicFinal = {};
    for oKey in oResultDic1.iterkeys():
        oResultDicFinal[string.atoi(oResultDic1[oKey][0])] = oResultDic1[oKey][1];
        
    for oKey in oResultDic2.iterkeys():
        oResultDicFinal[string.atoi(oResultDic2[oKey][0])] = oResultDic2[oKey][1];
    
    OutputHeader();
    
    oKeys = oResultDicFinal.keys();
    oKeys.sort();
    for oKey in oKeys:
        PrintToStdOut(oResultDicFinal[oKey]);
        
    OutputTailer();

    return 0;

if __name__ == "__main__":
    main();
    #print "¹þ¹þ";
