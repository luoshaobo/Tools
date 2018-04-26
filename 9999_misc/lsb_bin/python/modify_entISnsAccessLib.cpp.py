#!/usr/bin/python
# -*- coding: UTF-8 -*-

# NOTE:
#     please execute this program in Chinese environment (by using AppLocale: Simple Chinese)

import sys;
import os;
import string;
import re;
import xml.dom.minidom;

NEW_LINE = "\r\n"

ENTISNS_CPP_PATHNAME     = """D:\casdev\WinCE\public\MMP_PROD\_NAV\NavSNS\Main\AccessLib\src\entISnsAccessLib.cpp.orig""";
ENTISNS_CPP_PATHNAME_NEW = """D:\casdev\WinCE\public\MMP_PROD\_NAV\NavSNS\Main\AccessLib\src\entISnsAccessLib.cpp""";

EXCLUED_FUNCTIONS = [
    "entISns_Initialize",
    "entISns_checkHandler",
];

strLastRetType = "";

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def IsExcludedFunc(strLine):
    for strExcludedFunc in EXCLUED_FUNCTIONS:
        strPatten = '^[ \t]*(SNS_RESULT_E)[ \t]+%s\(.*$' % (strExcludedFunc);
        oRe = re.compile(strPatten);
        mo = oRe.match(strLine);
        if mo != None:
            return True;
    return False;
    
def IsFunc(strLine):
    strPatten = r'^[ \t]*(SNS_RESULT_E)[ \t]+(entISns)[^(]+\(.*$';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine);
    if mo != None:
        if IsExcludedFunc(strLine):
            return False;
        else:
            return True;
    else:
        return False;

def ExtractFuncNameAndArgs(strLine):
    global strLastRetType;
    
    strLastRetType = "";
    oList = [];
        
    strPatten = r'(^[ \t]*(SNS_RESULT_E)[ \t]+((entISns)[^(]+)\(.*$)';
    strReplace = r'\3';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine);
    if mo == None:
        return None;
    strFuncName = oRe.sub(strReplace, strLine);
    strFuncName = strFuncName.strip();
    oList.append(strFuncName);
    
    strPatten = r'(^[ \t]*(SNS_RESULT_E)[ \t]+(entISns)[^(]+\(([^)]*)\).*$)';
    strReplace = r'\4';
    strReplace2 = r'\2';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine);
    if mo == None:
        return None;
    strLastRetType = oRe.sub(strReplace2, strLine);
    strLastRetType = strLastRetType.strip(" \t\r\n");
    strArgumentsOrig = oRe.sub(strReplace, strLine);
    strArgumentsOrig2 = ""
    for ch in strArgumentsOrig:
        if ch == "*" or ch == "&" or ch == "[" or ch == "]":
            strArgumentsOrig2 += " ";
        else:
            strArgumentsOrig2 += ch;
    strArgumentsOrig = strArgumentsOrig2;
    oArgsList = strArgumentsOrig.split(",");
    if oArgsList != None:
        for strArg in oArgsList:
            oArgPartList = strArg.split();
            if oArgPartList != None and len(oArgPartList) >= 1:
                strArgChild = oArgPartList[-1];
                oArgPart2List = strArgChild.split("[");
                oList.append(oArgPart2List[0]);
    
    if len(oList) == 2 and oList[1] == "void":
        return oList[:-1];
    else: 
        return oList;
    
def MakeFuncCallStr(oFuncNameAndArgsList):
    strResult = "";
    strResult += "%s(" % (oFuncNameAndArgsList[0]);
    bFirstArg = True;
    for item in oFuncNameAndArgsList[1:]:
        if bFirstArg:
            bFirstArg = False;
        else:
            strResult += ", ";
        strResult += "%s" % (item);
    strResult += ")";
    return strResult;

def ProcessFile(strFilePath):
    global strLastRetType;
    
    strOutputFileContent = "";
    oFuncNameAndArgs = [];
    
    oFile = file(strFilePath, "r");
    if oFile == None:
        LOG_ERR("*** ERROR: ProcessFile(): can't open file \"%s\" to read!%s" % (strFilePath, NEW_LINE));
        exit(-1);
    bIsFunc = False;
    while True:
        strLine = oFile.readline();
        if strLine == "":                     # EOF
            break;
        if IsFunc(strLine):
            bIsFunc = True;
            oFuncNameAndArgs = ExtractFuncNameAndArgs(strLine);
            if oFuncNameAndArgs == None or oFuncNameAndArgs[0] == None:
                LOG_ERR("*** ERROR: ProcessFile(): can't extract function name and arguments from the line \"%s\"!%s" % (strLine, NEW_LINE));
                exit(-1);
            strOutputFileContent += strLine;
        elif bIsFunc and strLine.rstrip() == "{":
            strOutputFileContent += strLine;
            strOutputFileContent += "#ifdef USER_TEST%s" % (NEW_LINE);
            strOutputFileContent += "    uint32_t retUser = HMIentISnsAccessLibUser::getInstance().%s;%s" % (MakeFuncCallStr(oFuncNameAndArgs), NEW_LINE);
            strOutputFileContent += "    if (retUser != RET_NOT_PROCESSED) {%s" % (NEW_LINE);
            if strLastRetType == "void":
                strOutputFileContent += "        return;%s" % (NEW_LINE);
            elif strLastRetType == "BOOL":
                strOutputFileContent += "        return (BOOL)retUser;%s" % (NEW_LINE);
            elif strLastRetType == "bool_t":
                strOutputFileContent += "        return (bool_t)(retUser != 0);%s" % (NEW_LINE);
            elif strLastRetType == "SNS_RESULT_E":
                strOutputFileContent += "        return (SNS_RESULT_E)retUser;%s" % (NEW_LINE);
            else:
                LOG_ERR("*** ERROR: ProcessFile(): invalid return type: \"%s\"!%s" % (strLastRetType, NEW_LINE));
                exit(-1);
            strOutputFileContent += "    }%s" % (NEW_LINE);
            strOutputFileContent += "#endif // #ifdef USER_TEST%s" % (NEW_LINE);
            strOutputFileContent += "    %s" % (NEW_LINE);
            oFuncNameAndArgs = [];
            bIsFunc = False;
        else:
            strOutputFileContent += strLine;
            if strLine.strip() == "#include \"entISnsAccessLib.h\"":
                strOutputFileContent += "#include \"entISnsAccessLib_user.h\"%s" % (NEW_LINE);
                strOutputFileContent += "#define USER_TEST%s" % (NEW_LINE);
            
    oFile = None;
    return strOutputFileContent;

def main():
    strNewFileContent = ProcessFile(ENTISNS_CPP_PATHNAME);
    oFile = file(ENTISNS_CPP_PATHNAME_NEW, "w+b");
    if oFile == None:
        LOG_ERR("*** ERROR: main(): can't open file \"%s\" to write!%s" % (ENTISNS_CPP_PATHNAME_NEW, NEW_LINE));
        exit(-1);
    oFile.write(strNewFileContent);
    oFile = None;
    return 0;
    
def Test1():
    strLine = """bool_t entISysVf_SetDarkMode(uint8_t etat_bp_dark);""";
    bRet = IsFunc(strLine);
    print bRet;
    
def Test2():
    strLine = """bool_t entISysVf_SetDarkMode(uint8_t etat_bp_dark);""";
    oList = ExtractFuncNameAndArgs(strLine);
    print oList;
        
if __name__ == "__main__":
    main();
    #Test2();


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
