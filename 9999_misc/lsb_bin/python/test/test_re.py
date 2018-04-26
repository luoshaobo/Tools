#!/usr/bin/python

import sys
import re

def main1():
    strSrc = """abce aa 11223 00348 dkfksd
"""
    strResult = re.sub(r"(^abce aa ([0-9]*) 00348 dkfksd$)", r"\2", strSrc)
    print strResult

def main(strFilePath):
    strPatten = r'(^#include[ \t]+"([^.]+\.h)"[ \t\r]*$)';
    strReplace = r'\2';
    oRe = re.compile(strPatten)
    
    oFile = file(strFilePath, "r");
    if oFile != None:
        while True:
            line = oFile.readline();
            line = line.rstrip("\n");
            if line == None or line == "":
                break;
            #sys.stdout.write(line)
            mo = oRe.match(line)
            if mo != None:
                strVal = oRe.sub(strReplace, line)
                output = "XXX" + strVal + "###";
                #sys.stdout.write(strVal)
                print strVal
        oFile = None
    oRe = None
                


if __name__ == "__main__":
    main("GUI_DPCA_FontInfo.h")
