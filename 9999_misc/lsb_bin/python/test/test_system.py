#!/usr/bin/python

import os

def Main():
    TestOsPopen3();

def TestOsSystem():
    nError = os.system("ls -l /");
    print "nError=%d" % (nError)
    
def TestOsPopen():
    oFile = os.popen("ls -l /", "r");
    if oFile != None:
        strContent = oFile.read();
        print strContent;
    oFile = None;
    
def TestOsPopen2():
    oFile = os.popen("ls -l /", "r");
    if oFile != None:
        while True:
            strLine = oFile.readline();
            if strLine == "":
                break;
            strLine = strLine.strip(" \t\r\n");
            print "%s" % (strLine);
    oFile = None;

def TestOsPopen3():
    oList = []
    oFile = os.popen("ls /", "r");
    if oFile != None:
        while True:
            strLine = oFile.readline();
            if strLine == "":
                break;
            strLine = strLine.strip(" \t\r\n");
            oList.append(strLine);
    oFile = None;
    print oList
    if "bin" in oList:
        print "bin is in oList!"
   
    
if __name__ == "__main__":
    Main();
