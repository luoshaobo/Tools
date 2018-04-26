#ifndef TSD_TEST_H__2763481094023042387828747823785
#define TSD_TEST_H__2763481094023042387828747823785

#include "stdafx.h"

class TsdTester
{
public:
    TsdTester();
    virtual ~TsdTester();
    static TsdTester &GetInstance();
    
    int ReadErg(int argc, TCHAR* argv[]);
    int WriteErg(int argc, TCHAR* argv[]);
    
    int SetDimming(int argc, TCHAR* argv[]);
    int SetDarkMode(int argc, TCHAR* argv[]);
    int SetScreenOrientation(int argc, TCHAR* argv[]);
    int GetLastTouchPosition(int argc, TCHAR* argv[]);
    int GetDgtId(int argc, TCHAR* argv[]);
    int SetDisable(int argc, TCHAR* argv[]);
    int SetEnable(int argc, TCHAR* argv[]);
    int SetDisableThenEnable(int argc, TCHAR* argv[]);
    int SetHmiReady(int argc, TCHAR* argv[]);
    int SetHmiNotReady(int argc, TCHAR* argv[]);
    int SetDebounceMode(int argc, TCHAR* argv[]);
    
    int SetPowerMode(int argc, TCHAR* argv[]);
    int SetPowerOffThenOn(int argc, TCHAR* argv[]);
    int SetPowerOffThenOn2(int argc, TCHAR* argv[]);
};

#endif // #ifndef TSD_TEST_H__2763481094023042387828747823785
