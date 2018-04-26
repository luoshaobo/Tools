#pragma once
#include "stdafx.h"
#include <windows.h>
#include <vector>

#if 0
typedef int (*pFunc4_cdecl)(int, int, int, int);
typedef int (__stdcall *pFunc4_stdcall)(int, int, int, int);
class HookDummyClass
{
public:
    void HookFunc(int, int, int);
};
typedef int (HookDummyClass::*pFunc3_thiscall)(int, int, int);
#endif // #if 0

//
// NOTE: the injected function must be satisfied with:
// 1) Completely self dependent.
// 2) Not dependent to absolute address.
// 3) The return value and arguments are all simple type, not the instance of struct/class or float value.
// 4) The number of the arguments must not be bigger than 4.
//    For non-static class member function, the number of the arguments must not be bigger than 3.
//
class HookerWithAsmFunc
{
public:
    struct HookExtInfo {
        void *pOrigHookFunc;
        BOOL (WINAPI *pSetEvent)(HANDLE hEvent);
        HANDLE hEvent;
        int (WINAPI *pLoadLibrary)(void *pLibPath);
        int nLoadLibraryRet;
        TCHAR pLoadLibraryArg_LibPath[256];
    };

public:
    HookerWithAsmFunc();
    virtual ~HookerWithAsmFunc(void) {}
    
    BOOL SetHookExtInfo(HookExtInfo *pHookExtInfo) { m_pHookExtInfo = pHookExtInfo; return TRUE; }
    virtual BOOL GetDetourFuncContent(std::vector<unsigned char> &vDetourFuncContent) = 0;

    static HookerWithAsmFunc *CreateInstance();

protected:
    HookExtInfo *m_pHookExtInfo;                    // NOTE: the address is in the destination process
};
