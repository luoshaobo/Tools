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

ENTNAVIGATION_CPP_PATHNAME = """D:\casdev\WinCE\public\MMP_PROD\_NAV\Main\AccessLib\src\entNavigation.cpp.orig""";
ENTNAVIGATION_CPP_PATHNAME_NEW = """D:\casdev\WinCE\public\MMP_PROD\_NAV\Main\AccessLib\src\entNavigation.cpp""";

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def IsFunc(strLine):
    strPatten = r'^[ \t]*(BOOL|NAV_API_RET_VAL)[ \t]+HMINavigationAccessLib::command_[^(]+\(.*$';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo != None:
        return True;
    else:
        return False;
    
def ExtractFuncNameAndArgs(strLine):
    oList = [];
    
    strPatten = r'(^[ \t]*(BOOL|NAV_API_RET_VAL)[ \t]+HMINavigationAccessLib::(command_[^(]+)\(.*$)';
    strReplace = r'\3';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo == None:
        return None;
    strFuncName = oRe.sub(strReplace, strLine);
    strFuncName = strFuncName.strip();
    oList.append(strFuncName);
    
    strPatten = r'(^[ \t]*(BOOL|NAV_API_RET_VAL)[ \t]+HMINavigationAccessLib::command_[^(]+\(([^)]*)\).*$)';
    strReplace = r'\3';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo == None:
        return None;
    strArgumentsOrig = oRe.sub(strReplace, strLine);
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
        
def ExtractRetType(strLine):
    strRetType = "BOOL";
    
    strPatten = r'(^[ \t]*(BOOL|NAV_API_RET_VAL)[ \t]+HMINavigationAccessLib::(command_[^(]+)\(.*$)';
    strReplace = r'\2';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo == None:
        return None;
    strRetType = oRe.sub(strReplace, strLine);
    strRetType = strRetType.strip();
    return strRetType;
    
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
    strOutputFileContent = "";
    oFuncNameAndArgs = [];
    strRetType = "";
    
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
            strRetType = ExtractRetType(strLine);
            if strRetType == None or strRetType == "":
                LOG_ERR("*** ERROR: ProcessFile(): can't extract return type from the line \"%s\"!%s" % (strLine, NEW_LINE));
                exit(-1);
            strOutputFileContent += strLine;
        elif bIsFunc and strLine.rstrip() == "{":
            strOutputFileContent += strLine;
            strOutputFileContent += "#ifdef USER_TEST%s" % (NEW_LINE);
            strOutputFileContent += "    uint32_t retUser = HMINavigationAccessLibUser::getInstance().%s;%s" % (MakeFuncCallStr(oFuncNameAndArgs), NEW_LINE);
            strOutputFileContent += "    if (retUser != RET_NOT_PROCESSED) {%s" % (NEW_LINE);
            strOutputFileContent += "        return (%s)retUser;%s" % (strRetType, NEW_LINE);
            strOutputFileContent += "    }%s" % (NEW_LINE);
            strOutputFileContent += "    %s" % (NEW_LINE);
            strOutputFileContent += "#endif // #ifdef USER_TEST%s" % (NEW_LINE);
            oFuncNameAndArgs = [];
            strRetType = "";
            bIsFunc = False;
        else:
            strOutputFileContent += strLine;
            if strLine.strip() == "#include \"entNavigation.h\"":
                strOutputFileContent += "#include \"entNavigation_user.h\"%s" % (NEW_LINE);
                strOutputFileContent += "#define USER_TEST%s" % (NEW_LINE);
            
    oFile = None;
    return strOutputFileContent;

def main():
    strNewFileContent = ProcessFile(ENTNAVIGATION_CPP_PATHNAME);
    oFile = file(ENTNAVIGATION_CPP_PATHNAME_NEW, "w+b");
    if oFile == None:
        LOG_ERR("*** ERROR: main(): can't open file \"%s\" to write!%s" % (ENTNAVIGATION_CPP_PATHNAME_NEW, NEW_LINE));
        exit(-1);
    oFile.write(strNewFileContent);
    oFile = None;
    return 0;
    
def Test1():
    strLine = """BOOL HMINavigationAccessLib::command_RenamePCam(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    bRet = IsFunc(strLine);
    print bRet;
    
def Test2():
    strLine = """BOOL HMINavigationAccessLib::command_RenamePCam(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    oList = ExtractFuncNameAndArgs(strLine);
    print oList;
    
def Test3():
    strLine = """BOOL HMINavigationAccessLib::command_RenamePCam(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    oList = ExtractFuncNameAndArgs(strLine);
    sResult = MakeFuncCallStr(oList);
    print sResult;
    
if __name__ == "__main__":
    main();
    #Test3();


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
