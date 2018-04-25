// TestLoadExternalSymbal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include "FuncHooker.h"

#if 0
#define LOG_MSG                 _tprintf
#define LOG_LINE()              _tprintf(TEXT("%s() [line:%d]\n"), __FUNCTION__, __LINE__)
#else
#define LOG_MSG(...)
#define LOG_LINE()
#endif

//--------------------------------------------------------------------------------------
#define _EV_TAG                                         TEXT("__ExternalVariable__")
#define _EV_(symbal)                                    symbal ## __ExternalVariable__

#define _EF_TAG                                         TEXT("__ExternalFunction__")
#define _EF_(symbal)                                    symbal ## __ExternalFunction__

#define _EC_TAG                                         TEXT("__ExternalClass__")
#define _EC_(symbal)                                    symbal ## __ExternalClass__

//--------------------------------------------------------------------------------------
#define _EXPV_TAG                                       TEXT("__ExportedVariable__")
#define _EXPV_(symbal)                                  symbal ## __ExportedVariable__

#define _EXPF_TAG                                       TEXT("__ExportedFunction__")
#define _EXPF_(symbal)                                  symbal ## __ExportedFunction__

#define _EXPC_TAG                                       TEXT("__ExportedClass__")
#define _EXPC_(symbal)                                  symbal ## __ExportedClass__

//--------------------------------------------------------------------------------------
#define _IMPV_TAG                                       TEXT("__ImportedVariable__")
#define _IMPV_(symbal)                                  symbal ## __ImportedVariable__

#define _IMPF_TAG                                       TEXT("__ImportedFunction__")
#define _IMPF_(symbal)                                  symbal ## __ImportedFunction__

#define _IMPC_TAG                                       TEXT("__ImportedClass__")
#define _IMPC_(symbal)                                  symbal ## __ImportedClass__

////////////////////////////////////////////////////////////////////////////////////////

class DestProcInfo
{
public:
    struct DEST_PROC_INFO {
        unsigned int nRefVarAddrBeforeRelocation;
        unsigned int nRefVarAddrAfterRelocation;
    };

public:
    DestProcInfo() : m_hFileMapping(NULL), m_pDestProcInfo(NULL) {}
    ~DestProcInfo() { Unmap(); }

    static DestProcInfo &GetInstance() {
        static DestProcInfo aDestProcInfo;
        return aDestProcInfo;
    }

    BOOL Map();
    BOOL Unmap();
    DEST_PROC_INFO *GetPtr() { return m_pDestProcInfo; }

private:
    HANDLE m_hFileMapping;
    DEST_PROC_INFO *m_pDestProcInfo;
};

BOOL DestProcInfo::Map()
{
    const TCHAR *FM_NAME_DEST_PROC_INFO = TEXT("FM_NAME_DEST_PROC_INFO");
    BOOL bSuc = FALSE;
    
    LOG_LINE();
    if (m_hFileMapping == NULL) {
        LOG_LINE();
        m_hFileMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DEST_PROC_INFO), FM_NAME_DEST_PROC_INFO);
        if (m_hFileMapping == NULL) {
            LOG_LINE();
            bSuc = FALSE;
            goto FAILED;
        }
    }

    LOG_LINE();
    if (m_pDestProcInfo == NULL) {
        LOG_LINE();
        m_pDestProcInfo = (DEST_PROC_INFO *)::MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(DEST_PROC_INFO));
        if (m_pDestProcInfo == NULL) {
            LOG_LINE();
            DWORD nError = ::GetLastError();
            LOG_MSG("nError=%d\n", nError);
            bSuc = FALSE;
            goto FAILED;
        }
    }

    LOG_LINE();
    bSuc = TRUE;
FAILED:
    return bSuc;
}

BOOL DestProcInfo::Unmap()
{
    BOOL bSuc = FALSE;

    if (m_pDestProcInfo != NULL) {
        bSuc = UnmapViewOfFile(m_pDestProcInfo);
        if (!bSuc) {
            bSuc = FALSE;
            goto FAILED;
        }
        m_pDestProcInfo = NULL;
    }
    
    if (m_hFileMapping != NULL) {
        ::CloseHandle(m_hFileMapping);
        m_hFileMapping = NULL;
    }

    bSuc = TRUE;
FAILED:
    return bSuc;
}

////////////////////////////////////////////////////////////////////////////////////////

class SymbolAddressParser
{
private:
    typedef std::map<std::tstring, unsigned int> RecordMap;
    typedef std::map<std::tstring, unsigned int>::iterator RecordIterator;
    typedef std::pair<std::tstring, unsigned int> RecordPair;
    enum ParseType {
        PT_NORMAL,
        PT_WITH_TAG,
        PT_WITH_REMOVING_TAG,
        PT_WITHOUT_TAG,
    };

public:
    SymbolAddressParser() {}
    ~SymbolAddressParser() {}
    
    void SetMapFilePath(const std::tstring &sMapFilePath) { m_sMapFilePath = sMapFilePath; }
    bool Parse();
    bool ParseWithTag(const std::tstring &sTag);
    bool ParseWithRemovingTag(const std::tstring &sTag);
    bool ParseWithoutTag(const std::tstring &sTag);
    unsigned int GetSymbolAddress(const std::tstring &sSymbol);
    void EnumAllSymbolBegin();
    bool EnumAllSymbolNext(std::tstring &sSymbal, unsigned int &nAddress);

private:
    bool ParseCommon(ParseType pt, const std::tstring &sTag);

private:
    char m_cLineBuf[1024];
    TCHAR m_cFuncNameBuf[1024];
    std::tstring m_sMapFilePath;
    RecordMap m_mapRecords;
    RecordIterator m_EnumAllSymbolIterator;
};

bool SymbolAddressParser::ParseCommon(ParseType pt, const std::tstring &sTag)
{
    bool bRet = false;
    FILE *pFile = NULL;
    unsigned int nFuncAddr;

    m_mapRecords.clear();

    pFile = fopen(tstr2str(m_sMapFilePath).c_str(), "r");
    if (pFile == NULL) {
        bRet = false;
        goto FAILED;
    }

    while (true) {
        char *pLine = fgets(m_cLineBuf, sizeof(m_cLineBuf), pFile);
        if (pLine == NULL) {
            if (feof(pFile)) {
                break;
            } else {
                bRet = false;
                goto FAILED;
            }
        }

        memset(m_cFuncNameBuf, 0, sizeof(m_cFuncNameBuf));
        nFuncAddr = 0;
        int nField = _stscanf(str2tstr(m_cLineBuf).c_str(), TEXT("%*s%s%x"), m_cFuncNameBuf, &nFuncAddr);
        if (nField == 2) {
            if (m_cFuncNameBuf[0] != TEXT('\0') && nFuncAddr != 0) {
                std::tstring sFuncName(m_cFuncNameBuf);
                if (sFuncName.substr(0, 3) == std::tstring(TEXT("??_"))) {                     // ignore the internal symbols
                    continue;
                }

                if (pt == PT_NORMAL) {
                    m_mapRecords.insert(RecordPair(m_cFuncNameBuf, nFuncAddr));
                } else if (pt == PT_WITH_TAG) {
                    std::tstring::size_type nPos = sFuncName.find(sTag);
                    if (nPos != std::tstring::npos) {
                        m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                    }
                } else if (pt == PT_WITH_REMOVING_TAG) {
                    std::tstring::size_type nPos = sFuncName.find(sTag);
                    if (nPos != std::tstring::npos) {
                        sFuncName.erase(nPos, sTag.length());
                        if (sTag == _EV_TAG) {                                          // modify variable name
                            std::tstring sTemp(TEXT("@@3PA"));
                            if (sFuncName.substr(nPos, sTemp.length()) == sTemp) {
                                sFuncName.erase(nPos, sTemp.length());
                                sFuncName.insert(nPos, TEXT("@@3"));
                            }
                        }
                        m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                    }
                    while (true) {
                        nPos = sFuncName.find(sTag);                                   // remove more tags for complex types
                        if (nPos != std::tstring::npos) {
                            sFuncName.erase(nPos, sTag.length());
                            m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                        } else {
                            break;
                        }
                    }
                } else if (pt == PT_WITHOUT_TAG) {
                    std::tstring::size_type nPos = sFuncName.find(sTag);
                    if (nPos == std::tstring::npos) {
                        m_mapRecords.insert(RecordPair(sFuncName, nFuncAddr));
                    }
                } else {
                    // do nothing
                }
            }
        }
    }

    bRet = true;

FAILED:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return bRet;
}

bool SymbolAddressParser::Parse()
{
    return ParseCommon(PT_NORMAL, TEXT(""));
}

bool SymbolAddressParser::ParseWithTag(const std::tstring &sTag)
{
    return ParseCommon(PT_WITH_TAG, sTag);
}

bool SymbolAddressParser::ParseWithRemovingTag(const std::tstring &sTag)
{
    return ParseCommon(PT_WITH_REMOVING_TAG, sTag);
}

bool SymbolAddressParser::ParseWithoutTag(const std::tstring &sTag)
{
    return ParseCommon(PT_WITHOUT_TAG, sTag);
}

unsigned int SymbolAddressParser::GetSymbolAddress(const std::tstring &sSymbol)
{
    unsigned int nAddress = 0;

    RecordIterator it = m_mapRecords.find(sSymbol);
    if (it != m_mapRecords.end()) {
        nAddress = it->second;
    }

    return nAddress;
}

void SymbolAddressParser::EnumAllSymbolBegin()
{
    m_EnumAllSymbolIterator = m_mapRecords.begin();
    
}

bool SymbolAddressParser::EnumAllSymbolNext(std::tstring &sSymbal, unsigned int &nAddress)
{
    if (m_EnumAllSymbolIterator != m_mapRecords.end()) {
        sSymbal = m_EnumAllSymbolIterator->first;
        nAddress = m_EnumAllSymbolIterator->second;
        ++m_EnumAllSymbolIterator;
        return true;
    } else {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32_X86) // for Win32 X86
#define EXE_FILE_NAME                   TEXT("TestLoadExternalSymbal_debugee.exe")
#define EXE_MAP_FILE_PATH               TEXT("TestLoadExternalSymbal_debugee.map")
#define DLL_FILE_PATH                   TEXT("TestLoadExternalSymbal_DllToInject.dll")
#define DLL_MAP_FILE_PATH               TEXT("TestLoadExternalSymbal_DllToInject.map")
#define DLL_RELOCATION_REF_VAR_NAME     TEXT("_nRelocationRefVar")
#elif defined(WINCE_ARM32) // for WinCE ARM32
#define EXE_FILE_NAME                   TEXT("TestLoadExternalSymbal_debugee.exe")
#define EXE_MAP_FILE_PATH               TEXT("\\release\\TestLoadExternalSymbal_debugee.map")
#define DLL_FILE_PATH                   TEXT("TestLoadExternalSymbal_DllToInject.dll")
#define DLL_MAP_FILE_PATH               TEXT("\\release\\TestLoadExternalSymbal_DllToInject.map")
#define DLL_RELOCATION_REF_VAR_NAME     TEXT("nRelocationRefVar")
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

DWORD WINAPI HookDestThread(LPVOID lpParameter);
DWORD WINAPI InjectDLLThread(LPVOID lpParameter);
extern int debuger_tmain(int argc, TCHAR* argv[]);
extern HANDLE g_hDebugeeProcess;
extern BOOL g_bDebugActiveProcessStop;

int _tmain(int argc, _TCHAR* argv[])
{
    BOOL bSuc;
    HANDLE hProcess;
    DWORD nProcessId;
    HANDLE hThread;

    hThread = CreateThread(NULL, 0, &HookDestThread, NULL, 0, NULL);
    if (hThread == NULL) {
        return -1;
    }
    CloseHandle(hThread);
    
    Sleep(100); // NOTE: wait for thread HookDestThread stated first.
    hThread = CreateThread(NULL, 0, &InjectDLLThread, NULL, 0, NULL);
    if (hThread == NULL) {
        return -1;
    }
    CloseHandle(hThread);
    
    bSuc = FuncHooker::GetProcessByName(EXE_FILE_NAME, hProcess, nProcessId);
    if (!bSuc) {
        return -1;
    }
    ::CloseHandle(hProcess);
    
    TCHAR sPid[20];
    _sntprintf(sPid, sizeof(sPid) / sizeof(TCHAR), TEXT("%u"), nProcessId);

    int debuger_argc = 3;
    TCHAR *debuger_argv[4];
    memset(debuger_argv, 0, sizeof(debuger_argv));
    debuger_argv[0] = argv[0];
    debuger_argv[1] = TEXT("-p");
    debuger_argv[2] = sPid;
    return debuger_tmain(debuger_argc, debuger_argv);
}

void WakeupDebuggee()
{
    static const TCHAR *EVT_NAME_DEBUG_WAIT = TEXT("EVT_NAME_DEBUG_WAIT");

    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EVT_NAME_DEBUG_WAIT);
    if (hEvent != NULL) {
        SetEvent(hEvent);
    }
}

void WaitForInjectionFinished()
{
    static const TCHAR *EVT_NAME_INJECTION_FINISHED = TEXT("EVT_NAME_INJECTION_FINISHED");

    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EVT_NAME_INJECTION_FINISHED);
    if (hEvent != NULL) {
        WaitForSingleObject(hEvent, INFINITE);
    }
}

void WaitForInjectionBegin()
{
    static const TCHAR *EVT_NAME_INJECTION_BEGIN = TEXT("EVT_NAME_INJECTION_BEGIN");

    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EVT_NAME_INJECTION_BEGIN);
    if (hEvent != NULL) {
        WaitForSingleObject(hEvent, INFINITE);
    }
}

void SetForInjectionBegin()
{
    static const TCHAR *EVT_NAME_INJECTION_BEGIN = TEXT("EVT_NAME_INJECTION_BEGIN");

    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EVT_NAME_INJECTION_BEGIN);
    if (hEvent != NULL) {
        SetEvent(hEvent);
    }
}

BOOL RelocateAddress(unsigned int pAddrIn, unsigned int *ppAddrOut) {
    BOOL bSuc = FALSE;

    if (ppAddrOut == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    DestProcInfo::DEST_PROC_INFO *pDestProcInfoPtr = DestProcInfo::GetInstance().GetPtr();
    if (pDestProcInfoPtr == NULL) {
        bSuc = FALSE;
        goto FAILED;
    }

    *ppAddrOut = (unsigned int)((unsigned int)pAddrIn - pDestProcInfoPtr->nRefVarAddrBeforeRelocation + pDestProcInfoPtr->nRefVarAddrAfterRelocation);

    bSuc = TRUE;
FAILED:
    return bSuc;
}

DWORD WINAPI HookDestThread(LPVOID lpParameter)
{
    BOOL bSuc;
    HANDLE hProcess = NULL;

    WaitForInjectionFinished();
    
    bSuc = DestProcInfo::GetInstance().Map();
    if (!bSuc) {
        return -1;
    }

    hProcess = g_hDebugeeProcess;
    if (hProcess == NULL) {
        return -1;
    }

    SymbolAddressParser sapExe;
    sapExe.SetMapFilePath(EXE_MAP_FILE_PATH);
    sapExe.Parse();

    {
        SymbolAddressParser sapDll;
        sapDll.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDll.ParseWithTag(DLL_RELOCATION_REF_VAR_NAME);

        std::tstring sSymbalName;
        unsigned int nTargetAddess;
        sapDll.EnumAllSymbolBegin();
        while (sapDll.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            if (sSymbalName == DLL_RELOCATION_REF_VAR_NAME) {
                DestProcInfo::DEST_PROC_INFO *pDestProcInfoPtr = DestProcInfo::GetInstance().GetPtr();
                if (pDestProcInfoPtr == NULL) {
                    return -1;
                }
                pDestProcInfoPtr->nRefVarAddrBeforeRelocation = nTargetAddess;
                break;
            }
        }
    }

    {
        FuncHookerHelper funcHookerHelper(hProcess, false);

        SymbolAddressParser sapDllExp;
        sapDllExp.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDllExp.ParseWithRemovingTag(_EXPF_TAG);

        SymbolAddressParser sapDllImp;
        sapDllImp.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDllImp.ParseWithRemovingTag(_IMPF_TAG);

        funcHookerHelper.HookTransactionBegin();
        std::tstring sSymbalName;
        unsigned int nDetourAddess;
        sapDllExp.EnumAllSymbolBegin();
        while (sapDllExp.EnumAllSymbolNext(sSymbalName, nDetourAddess)) {
            RelocateAddress(nDetourAddess, &nDetourAddess);
            unsigned int nTargetAddess = (unsigned int)sapExe.GetSymbolAddress(sSymbalName);
            unsigned int nfTrampolineProxy = (unsigned int)sapDllImp.GetSymbolAddress(sSymbalName);
            RelocateAddress(nfTrampolineProxy, &nfTrampolineProxy);
            if (nDetourAddess != NULL && nTargetAddess != NULL && nfTrampolineProxy != NULL) {
                funcHookerHelper.AddDualHooker((void *)nTargetAddess, (void *)nDetourAddess, (void *)nfTrampolineProxy);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }

    {
        FuncHookerHelper funcHookerHelper(hProcess, false);

        SymbolAddressParser sapDllExp;
        sapDllExp.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDllExp.ParseWithRemovingTag(_EXPC_TAG);

        SymbolAddressParser sapDllImp;
        sapDllImp.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDllImp.ParseWithRemovingTag(_IMPC_TAG);

        funcHookerHelper.HookTransactionBegin();
        std::tstring sSymbalName;
        unsigned int nDetourAddess;
        sapDllExp.EnumAllSymbolBegin();
        while (sapDllExp.EnumAllSymbolNext(sSymbalName, nDetourAddess)) {
            RelocateAddress(nDetourAddess, &nDetourAddess);
            unsigned int nTargetAddess = (unsigned int)sapExe.GetSymbolAddress(sSymbalName);
            unsigned int nfTrampolineProxy = (unsigned int)sapDllImp.GetSymbolAddress(sSymbalName);
            RelocateAddress(nfTrampolineProxy, &nfTrampolineProxy);
            if (nDetourAddess != NULL && nTargetAddess != NULL && nfTrampolineProxy != NULL) {
                funcHookerHelper.AddDualHooker((void *)nTargetAddess, (void *)nDetourAddess, (void *)nfTrampolineProxy);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }
    
    {
        FuncHookerHelper funcHookerHelper(hProcess, false);
        SymbolAddressParser sapDll;
        sapDll.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDll.ParseWithRemovingTag(_EF_TAG);

        funcHookerHelper.HookTransactionBegin();
        std::tstring sSymbalName;
        unsigned int nTargetAddess;
        sapDll.EnumAllSymbolBegin();
        while (sapDll.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            RelocateAddress(nTargetAddess, &nTargetAddess);
            unsigned int nDetourAddess = (unsigned int)sapExe.GetSymbolAddress(sSymbalName);
            if (nDetourAddess != NULL && nTargetAddess != NULL) {
                funcHookerHelper.AddHooker((void *)nTargetAddess, (void *)nDetourAddess, NULL);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }
    
    {
        FuncHookerHelper funcHookerHelper(hProcess, false);
        SymbolAddressParser sapDll;
        sapDll.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDll.ParseWithRemovingTag(_EC_TAG);

        funcHookerHelper.HookTransactionBegin();
        std::tstring sSymbalName;
        unsigned int nTargetAddess;
        sapDll.EnumAllSymbolBegin();
        while (sapDll.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            RelocateAddress(nTargetAddess, &nTargetAddess);
            unsigned int nDetourAddess = (unsigned int)sapExe.GetSymbolAddress(sSymbalName);
            if (nDetourAddess != NULL && nTargetAddess != NULL) {
                funcHookerHelper.AddHooker((void *)nTargetAddess, (void *)nDetourAddess, NULL);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }
    
    {
        FuncHookerHelper funcHookerHelper(hProcess, false);
        SymbolAddressParser sapDll;
        sapDll.SetMapFilePath(DLL_MAP_FILE_PATH);
        sapDll.ParseWithRemovingTag(_EV_TAG);

        funcHookerHelper.HookTransactionBegin();
        std::tstring sSymbalName;
        unsigned int nTargetAddess;
        sapDll.EnumAllSymbolBegin();
        while (sapDll.EnumAllSymbolNext(sSymbalName, nTargetAddess)) {
            RelocateAddress(nTargetAddess, &nTargetAddess);
            unsigned int nDetourAddess = (unsigned int)sapExe.GetSymbolAddress(sSymbalName);
            if (nDetourAddess != NULL && nTargetAddess != NULL) {
                ::WriteProcessMemory(hProcess, (LPVOID)nTargetAddess, &nDetourAddess, 4, NULL);
            }
        }
        funcHookerHelper.HookTransactionCommit();
    }

    WakeupDebuggee();

    g_bDebugActiveProcessStop = TRUE;

    return 0;
}

#if HOOK_METHOD == INJECT_METHON__DIRECTLY
DWORD WINAPI InjectDLLThread(LPVOID lpParameter)
{
    HANDLE hProcess;
    BOOL bSuc;

    WaitForInjectionBegin();

    hProcess = g_hDebugeeProcess;
    if (hProcess == NULL) {
        return -1;
    }

    bSuc = FuncHooker::LoadLibraryInProcess(DLL_FILE_PATH), hProcess, 0);
    if (!bSuc) {
        return -1;
    }

    return 0;
}
#elif HOOK_METHOD == INJECT_METHON__DIRECTLY_WITH_HOOK
DWORD WINAPI InjectDLLThread(LPVOID lpParameter)
{
    HANDLE hProcess;
    BOOL bSuc;
    unsigned int nHookDestFuncAddr;

    WaitForInjectionBegin();

    hProcess = g_hDebugeeProcess;
    if (hProcess == NULL) {
        return -1;
    }

    SymbolAddressParser sapExe;
    sapExe.SetMapFilePath(EXE_MAP_FILE_PATH);
    sapExe.Parse();

#if defined(WIN32_X86) // for Win32 X86
    nHookDestFuncAddr = sapExe.GetSymbolAddress(TEXT("?FuncToBeHookedForDllInjection@FuncToBeHookedForDllInjectionClass@@QAEHHHH@Z"));
#elif defined(WINCE_ARM32) // for WinCE ARM32
    nHookDestFuncAddr = sapExe.GetSymbolAddress(TEXT("?FuncToBeHookedForDllInjection@FuncToBeHookedForDllInjectionClass@@QAAHHHH@Z"));
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
    
    if (nHookDestFuncAddr == 0) {
        return -1;
    }

    bSuc = FuncHooker::LoadLibraryInProcess(DLL_FILE_PATH, hProcess, nHookDestFuncAddr);
    if (!bSuc) {
        return -1;
    }

    return 0;
}
#elif HOOK_METHOD == INJECT_METHON__BY_SETWINDOWSHOOKEX
#define MONITORED_WINDOW_NAME       TEXT("HookedWindow")
DWORD WINAPI InjectDLLThread(LPVOID lpParameter)
{
    HWND hWndDest;

    WaitForInjectionBegin();

    //for (;;) {
        hWndDest = FindWindow(NULL, MONITORED_WINDOW_NAME);
        if (hWndDest == NULL) {
            Sleep(1000);
            //continue;
        } else {
            //SendMessage(hWndDest, WM_KEYDOWN, VK_F2, 0xC03C0001);
            SendMessage(hWndDest, WM_USER, 0, 0);                                   // NOTE: The hook with type WH_CALLWNDPROC will not be executed by PostMessage(). 
            //EnableHardwareKeyboard(FALSE);
            //keybd_event((BYTE)VK_F2, (BYTE)0xC03C0001, 0, 0);
            //EnableHardwareKeyboard(TRUE);
            Sleep(100);
        }
    //}

    return 0;
}
#else
#error HOOK_METHOD is undefinded
#endif // #if 0
