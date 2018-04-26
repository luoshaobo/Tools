#!/usr/bin/env python

import sys;
import os;
import string;
from pprint import pprint

import Temp_AppData

NEW_LINE = "\r\n";

DEF_TEXT_FRAME_SWAPPED = "frameSwapped";

def LOG_INF(strMsg):
    sys.stderr.write(strMsg);

def LOG_ERR(strMsg):
    sys.stderr.write(strMsg);
    
def write_str_to_file(strContent, strFilePath):
    oFile = file(strFilePath, "w+b");
    if oFile != None:
        oFile.write(strContent);
    oFile = None;

def mapResult_cmp(x, y):
    return -cmp(x, y);
    
def Avg(oFloatList):
    fAvg = 0.0;
    fSum = 0.0;
    for fItem in oFloatList:
        fSum += fItem;
    if len(oFloatList) != 0:
        fAvg = fSum / len(oFloatList);
    return fAvg;
    
def OutputResult(mapResult):
    #pprint(mapResult);
    
    mapResult2 = {};
    for key in mapResult.keys():
        arrTimes = mapResult[key];
        arrTimes.sort(mapResult_cmp);
        fAvg = Avg(arrTimes);
        arrNewTimes = arrTimes;
        arrNewTimes.insert(0, fAvg);
        mapResult2[key] = arrNewTimes;
    #pprint(mapResult2);
    
    sExcelFormatResult = "";
    keySorted = mapResult2.keys();
    keySorted.sort();
    for key in keySorted:
        sTimes = "";
        for fTime in mapResult2[key]:
            if len(sTimes) != 0:
                sTimes += "\t";
            sTimes += "%.3f" % (fTime);
        sExcelFormatResult += "%s\t%s\r\n" % (key, sTimes);
    print(sExcelFormatResult);
    
def DoProcess():
    mapResult = {};
    arrLastScreenInfo = [];
    
    for oItem in Temp_AppData.arrItems:
        fTime = oItem["time"];
        sText = oItem["text"];
        sText = sText.strip();
        
        if len(sText) == 0:
            continue;
        elif sText != DEF_TEXT_FRAME_SWAPPED:
            arrLastScreenInfo.append({"time": fTime, "text": sText});
        else:
            if len(arrLastScreenInfo) != 0:
                for oLastScreenInfo in arrLastScreenInfo:
                    fDeltaTime = fTime - oLastScreenInfo["time"];
                    arrTime = mapResult.get(oLastScreenInfo["text"]);
                    if arrTime == None:
                        arrTime = [];
                    arrTime.append(fDeltaTime)
                    mapResult[oLastScreenInfo["text"]] = arrTime;
                arrLastScreenInfo = [];
    
    OutputResult(mapResult);
    return 0;
    
def main():
    DoProcess();
    return 0;       
    
if __name__ == "__main__":
     main();
