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

TO_BE_CHANGED_FILES = [
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAasRvc.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAasRvc.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAudio.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAudio.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAudioVideo.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvAudioVideo.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvCitypark.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvCitypark.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvSource.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvSource.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvSpeech.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvSpeech.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvVideo.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvVideo.cpp"""
    ],
    
    [
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvCloudVr.cpp.orig""",
        """D:\casdev\WinCE\public\MMP_PROD\_AV\Main\AccessLib\src\entIAvCloudVr.cpp"""
    ],
];

ExcludedFuntions = [
    "entIAV_GetAasData",
    "entIAV_VHRInitialize",
];

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def IsExcludedFuncton(strFuncName):
    for F in ExcludedFuntions:
        if F == strFuncName:
            return True;
    return False;
    
def IsFunc(strLine):
    strPatten = r'^[ \t]*(bool_t)[ \t]+entIAV_[^(]+\(.*$';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo != None:
        strPatten = r'(^[ \t]*(bool_t)[ \t]+(entIAV_[^(]+)\(.*$)';
        strReplace = r'\3';
        oRe = re.compile(strPatten);
        mo = oRe.match(strLine)
        if mo == None:
            return None;
        strFuncName = oRe.sub(strReplace, strLine);
        strFuncName = strFuncName.strip();
        if IsExcludedFuncton(strFuncName):
            return False;
        return True;
    else:
        return False;
    
def ExtractFuncNameAndArgs(strLine):
    oList = [];
    
    strPatten = r'(^[ \t]*(bool_t)[ \t]+(entIAV_[^(]+)\(.*$)';
    strReplace = r'\3';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo == None:
        return None;
    strFuncName = oRe.sub(strReplace, strLine);
    strFuncName = strFuncName.strip();
    oList.append(strFuncName);
    
    strPatten = r'(^[ \t]*(bool_t)[ \t]+entIAV_[^(]+\(([^)]*)\).*$)';
    strReplace = r'\3';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo == None:
        return None;
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
        
def ExtractRetType(strLine):
    strRetType = "BOOL";
    
    strPatten = r'(^[ \t]*(bool_t)[ \t]+entIAV_[^(]+\(.*$)';
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
                LOG_ERR("*** ERROR: ProcessFile(): [%s] can't extract function name and arguments from the line \"%s\"!%s" % (strFilePath, strLine, NEW_LINE));
                exit(-1);
            strRetType = ExtractRetType(strLine);
            if strRetType == None or strRetType == "":
                LOG_ERR("*** ERROR: ProcessFile(): [%s] can't extract return type from the line \"%s\"!%s" % (strFilePath, strLine, NEW_LINE));
                exit(-1);
            strOutputFileContent += strLine;
        elif bIsFunc and strLine.rstrip() == "{":
            strOutputFileContent += strLine;
            strOutputFileContent += "#ifdef USER_TEST%s" % (NEW_LINE);
            strOutputFileContent += "    int retUser = HMIAvAudioVideoAccessLibUser::getInstance().%s;%s" % (MakeFuncCallStr(oFuncNameAndArgs), NEW_LINE);
            strOutputFileContent += "    if (retUser != RET_NOT_PROCESSED) {%s" % (NEW_LINE);
            strOutputFileContent += "        return retUser == 0 ? FALSE : TRUE;%s" % (NEW_LINE);
            strOutputFileContent += "    }%s" % (NEW_LINE);
            strOutputFileContent += "    %s" % (NEW_LINE);
            strOutputFileContent += "#endif // #ifdef USER_TEST%s" % (NEW_LINE);
            oFuncNameAndArgs = [];
            strRetType = "";
            bIsFunc = False;
        elif bIsFunc and strLine.rstrip() != "{":
            strOutputFileContent += strLine;
            strOutputFileContent += "#ifdef USER_TEST%s" % (NEW_LINE);
            strOutputFileContent += "    int retUser = HMIAvAudioVideoAccessLibUser::getInstance().%s;%s" % (MakeFuncCallStr(oFuncNameAndArgs), NEW_LINE);
            strOutputFileContent += "    if (retUser != RET_NOT_PROCESSED) {%s" % (NEW_LINE);
            strOutputFileContent += "        return retUser == 0 ? FALSE : TRUE;%s" % (NEW_LINE);
            strOutputFileContent += "    }%s" % (NEW_LINE);
            strOutputFileContent += "    %s" % (NEW_LINE);
            strOutputFileContent += "#endif // #ifdef USER_TEST%s" % (NEW_LINE);
            oFuncNameAndArgs = [];
            strRetType = "";
            bIsFunc = False;
        else:
            strOutputFileContent += strLine;
            if strLine.strip() == "#include \"entIAvAudioVideo.h\"":
                strOutputFileContent += "#include \"entIAvAudioVideo_user.h\"%s" % (NEW_LINE);
                strOutputFileContent += "#define USER_TEST%s" % (NEW_LINE);
            
    oFile = None;
    return strOutputFileContent;

def main():
    for FilePair in TO_BE_CHANGED_FILES:
        ORIG_FILE = FilePair[0];
        NEW_FILE = FilePair[1];
        strNewFileContent = ProcessFile(ORIG_FILE);
        oFile = file(NEW_FILE, "w+b");
        if oFile == None:
            LOG_ERR("*** ERROR: main(): can't open file \"%s\" to write!%s" % (NEW_FILE, NEW_LINE));
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


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
