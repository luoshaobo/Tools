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

ENTTELEMATICS_CPP_PATHNAME = """D:\casdev\WinCE\public\MMP_PROD\_TELE\AccessLib\src\entITelematics.cpp.orig""";
ENTTELEMATICS_CPP_PATHNAME_NEW = """D:\casdev\WinCE\public\MMP_PROD\_TELE\AccessLib\src\entITelematics.cpp""";

EXCLUED_FUNCTIONS = [
    "entITelematics_checkHandler",
];

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def IsExcludedFunc(strLine):
    for strExcludedFunc in EXCLUED_FUNCTIONS:
        strPatten = '^[ \t]*E_TLM_RESULT[ \t]+%s\(.*$' % (strExcludedFunc);
        oRe = re.compile(strPatten);
        mo = oRe.match(strLine);
        if mo != None:
            return True;
    return False;
    
def IsFunc(strLine):
    strPatten = r'^[ \t]*E_TLM_RESULT[ \t]+entITelematics[^(]+\(.*$';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo != None:
        if IsExcludedFunc(strLine):
            return False;
        else:
            return True;
    else:
        return False;
    
def ExtractFuncNameAndArgs(strLine):
    oList = [];
    
    strPatten = r'(^[ \t]*E_TLM_RESULT[ \t]+(entITelematics[^(]+)\(.*$)';
    strReplace = r'\2';
    oRe = re.compile(strPatten);
    mo = oRe.match(strLine)
    if mo == None:
        return None;
    strFuncName = oRe.sub(strReplace, strLine);
    strFuncName = strFuncName.strip();
    oList.append(strFuncName);
    
    strPatten = r'(^[ \t]*E_TLM_RESULT[ \t]+entITelematics[^(]+\(([^)]*)\).*$)';
    strReplace = r'\2';
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
            strOutputFileContent += "    uint32_t retUser = HMITelematicsAccessLibUser::getInstance().%s;%s" % (MakeFuncCallStr(oFuncNameAndArgs), NEW_LINE);
            strOutputFileContent += "    if (retUser != RET_NOT_PROCESSED) {%s" % (NEW_LINE);
            strOutputFileContent += "        return (E_TLM_RESULT)retUser;%s" % (NEW_LINE);
            strOutputFileContent += "    }%s" % (NEW_LINE);
            strOutputFileContent += "    %s" % (NEW_LINE);
            strOutputFileContent += "#endif // #ifdef USER_TEST%s" % (NEW_LINE);
            oFuncNameAndArgs = [];
            bIsFunc = False;
        else:
            strOutputFileContent += strLine;
            if strLine.strip() == "#include \"entITelematics.h\"":
                strOutputFileContent += "#include \"entITelematics_user.h\"%s" % (NEW_LINE);
                strOutputFileContent += "#define USER_TEST%s" % (NEW_LINE);
            
    oFile = None;
    return strOutputFileContent;

def main():
    strNewFileContent = ProcessFile(ENTTELEMATICS_CPP_PATHNAME);
    oFile = file(ENTTELEMATICS_CPP_PATHNAME_NEW, "w+b");
    if oFile == None:
        LOG_ERR("*** ERROR: main(): can't open file \"%s\" to write!%s" % (ENTTELEMATICS_CPP_PATHNAME_NEW, NEW_LINE));
        exit(-1);
    oFile.write(strNewFileContent);
    oFile = None;
    return 0;
    
def Test1():
    strLine = """E_TLM_RESULT entITelematicsGetUrlBackData(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    bRet = IsFunc(strLine);
    print bRet;
    
def Test2():
    strLine = """E_TLM_RESULT entITelematicsGetUrlBackData(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    oList = ExtractFuncNameAndArgs(strLine);
    print oList;
    
def Test2_1():
    strLine = """E_TLM_RESULT entITelematicsGetUrlBackData(void) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    oList = ExtractFuncNameAndArgs(strLine);
    print oList;
    
def Test3():
    strLine = """E_TLM_RESULT entITelematicsGetUrlBackData(short index, PCamName name) const /*DG CPP2ISQP-ISQPcpp Rule 8.7.1-SCC_GRANT_0006 */""";
    oList = ExtractFuncNameAndArgs(strLine);
    sResult = MakeFuncCallStr(oList);
    print sResult;
    
if __name__ == "__main__":
    main();
    #Test2_1();


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
