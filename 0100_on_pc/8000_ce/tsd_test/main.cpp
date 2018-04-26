// TestXML001.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "TsdTester.h"

int usage(int argc, TCHAR* argv[])
{
    printf("Usage:\n");
    printf("  %s ReadErg <nRegId>\n", wstr2str(argv[0]).c_str());
    printf("  %s WriteErg <nRegId> <nValueByte1> ...\n", wstr2str(argv[0]).c_str());
    
    printf("  %s SetDimming <u8Dimming>\n", wstr2str(argv[0]).c_str());
    printf("  %s SetDarkMode <u8Darkmode>\n", wstr2str(argv[0]).c_str());
    printf("  %s SetScreenOrientation <u8ScreenOrientation>\n", wstr2str(argv[0]).c_str());
    printf("  %s GetLastTouchPosition\n", wstr2str(argv[0]).c_str());
    printf("  %s GetDgtId\n", wstr2str(argv[0]).c_str());
    printf("  %s SetDisable\n", wstr2str(argv[0]).c_str());
    printf("  %s SetEnable\n", wstr2str(argv[0]).c_str());
    printf("  %s SetDisableThenEnable <nIntervalMillicSeconds>\n", wstr2str(argv[0]).c_str());
    printf("  %s SetHmiReady\n", wstr2str(argv[0]).c_str());
    printf("  %s SetHmiNotReady\n", wstr2str(argv[0]).c_str());
    printf("  %s SetDebounceMode <u8DebounceMode>\n", wstr2str(argv[0]).c_str());
    printf("  %s SetPowerMode <u8PowerMode>\n", wstr2str(argv[0]).c_str());
    printf("  %s SetPowerOffThenOn <nIntervalMillicSeconds>\n", wstr2str(argv[0]).c_str());
    printf("  %s SetPowerOffThenOn2 <nIntervalMillicSeconds>\n", wstr2str(argv[0]).c_str());
    printf("\n");
    
    return 0;
}

int wmain(int argc, TCHAR* argv[])
{
	int nRet = 0;
    TsdTester &rTsdTester = TsdTester::GetInstance();
    std::string sCmd;
    
    if (argc >= 2) {
        sCmd = wstr2str(argv[1]);
    }
    
    if (sCmd == "ReadErg") {
        nRet = rTsdTester.ReadErg(argc, argv);
    } else if (sCmd == "WriteErg") {
        nRet = rTsdTester.WriteErg(argc, argv);
    } else if (sCmd == "SetDimming") {
        nRet = rTsdTester.SetDimming(argc, argv);
    } else if (sCmd == "SetDarkMode") {
        nRet = rTsdTester.SetDarkMode(argc, argv);
    } else if (sCmd == "SetScreenOrientation") {
        nRet = rTsdTester.SetScreenOrientation(argc, argv);
    } else if (sCmd == "GetLastTouchPosition") {
        nRet = rTsdTester.GetLastTouchPosition(argc, argv);
    } else if (sCmd == "GetDgtId") {
        nRet = rTsdTester.GetDgtId(argc, argv);
    } else if (sCmd == "SetDisable") {
        nRet = rTsdTester.SetDisable(argc, argv);
    } else if (sCmd == "SetEnable") {
        nRet = rTsdTester.SetEnable(argc, argv);
    } else if (sCmd == "SetDisableThenEnable") {
        nRet = rTsdTester.SetDisableThenEnable(argc, argv);
    } else if (sCmd == "SetHmiReady") {
        nRet = rTsdTester.SetHmiReady(argc, argv);
    } else if (sCmd == "SetHmiNotReady") {
        nRet = rTsdTester.SetHmiNotReady(argc, argv);
    } else if (sCmd == "SetDebounceMode") {
        nRet = rTsdTester.SetDebounceMode(argc, argv);
    } else if (sCmd == "SetPowerMode") {
        nRet = rTsdTester.SetPowerMode(argc, argv);
    } else if (sCmd == "SetPowerOffThenOn") {
        nRet = rTsdTester.SetPowerOffThenOn(argc, argv);
    } else if (sCmd == "SetPowerOffThenOn2") {
        nRet = rTsdTester.SetPowerOffThenOn2(argc, argv);
    } else {
        nRet = usage(argc, argv);
    }
    
	return nRet;
}
