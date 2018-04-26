// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "TestLoadExternalSymbal_DllToInject.h"

#if 0
#define LOG_MSG                 _tprintf
#define LOG_LINE()              _tprintf(TEXT("%S() [line:%d]\n"), __FUNCTION__, __LINE__)
#else
#define LOG_MSG(...)
#define LOG_LINE()
#endif

////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

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

//--------------------------------------------------------------------------------------

#define CCC_SIZE                                        1024

class _EC_(CCC) {
public:
    _EC_(CCC)();
    ~_EC_(CCC)();
    
    static _EC_(CCC) &GetInstance();
    static _EC_(CCC) &GetInstance(int nPlaceHolder);

    virtual int PubF(char *p);

private:
    unsigned char m_RealObject[CCC_SIZE];       // NOTE: this member must be the first data member!
};


_EC_(CCC)::_EC_(CCC)()
{
    printf("in %s()\n", __FUNCTION__);
}

_EC_(CCC)::~_EC_(CCC)()
{
    printf("in %s()\n", __FUNCTION__);
}

_EC_(CCC) &_EC_(CCC)::GetInstance()
{
    printf("in %s()\n", __FUNCTION__);
    static _EC_(CCC) oThis;
    return oThis;
}

_EC_(CCC) &_EC_(CCC)::GetInstance(int nPlaceHolder)
{
    printf("in %s()\n", __FUNCTION__);
    static _EC_(CCC) oThis;
    return oThis;
}

int _EC_(CCC)::PubF(char *p)
{
    printf("in %s()\n", __FUNCTION__);
    return -1;
}

//--------------------------------------------------------------------------------------

int _IMPF_(GGG)(int n)
{
    printf("in %s(): n=%d\n", __FUNCTION__, n);
    return 0;
}

int _EXPF_(GGG)(int n)
{
    printf("in %s(): n=%d\n", __FUNCTION__, n);
    return _IMPF_(GGG)(n);
}

//--------------------------------------------------------------------------------------
class _IMPC_(TTT)
{
public:
    int PubF(char *p);
};

int _IMPC_(TTT)::PubF(char *p)
{
    printf("in %s(\"%s\")\n", __FUNCTION__, p);
    return 0;
}

class _EXPC_(TTT)
{
public:
    int PubF(char *p);
};

int _EXPC_(TTT)::PubF(char *p)
{
    printf("in %s(\"%s\")\n", __FUNCTION__, p);
    return ((_IMPC_(TTT) *)this)->PubF(p);
}

////////////////////////////////////////////////////////////////////////////////////////

int _EF_(FFF)(int n)
{
    printf("in %s()\n", __FUNCTION__);
    return -1;
}

int *_EV_(VVVn) = NULL;
char *_EV_(VVVc) = NULL;
short *_EV_(VVVs) = NULL;

struct {
    const char *name;
    unsigned int age;
    bool sex;
} *_EV_(VVVst_Girl) = NULL;

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
            LOG_MSG(TEXT("nError=%d\n"), nError);
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

void SendInjectionFinished()
{
    static const TCHAR *EVT_NAME_INJECTION_FINISHED = TEXT("EVT_NAME_INJECTION_FINISHED");

    LOG_LINE();
    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EVT_NAME_INJECTION_FINISHED);
    if (hEvent != NULL) {
        LOG_LINE();
        SetEvent(hEvent);
    }
}

void WaitForDebugger()
{
    static const TCHAR *EVT_NAME_DEBUG_WAIT = TEXT("EVT_NAME_DEBUG_WAIT");

    static HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, EVT_NAME_DEBUG_WAIT);
    if (hEvent != NULL) {
        WaitForSingleObject(hEvent, INFINITE);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI TestThread(LPVOID lpParameter)
{
    printf("Wait for debugger...\n");
    WaitForDebugger();
    printf("OK\n");

    {
        _EC_(CCC) ccc;
        ccc.PubF("created by constructor");
    }

    printf("----------------------------------------------\n");

    {
        _EC_(CCC) &rccc = _EC_(CCC)::GetInstance();
        rccc.PubF("created by GetInstance()");
    }

    printf("----------------------------------------------\n");

    {
        _EC_(CCC) &rccc = _EC_(CCC)::GetInstance(0);
        rccc.PubF("created by inline GetInstance()");
    }

    printf("----------------------------------------------\n");

    {
        _EF_(FFF)(123);
    }

    printf("----------------------------------------------\n");

    {
        if (_EV_(VVVn) != NULL) {
            printf("*_EV_(VVVn)=%d\n", *_EV_(VVVn));
        } else {
            printf("_EV_(VVVn)=NULL\n");
        }

        if (_EV_(VVVc) != NULL) {
            printf("*_EV_(VVVc)=%c\n", *_EV_(VVVc));
        } else {
            printf("_EV_(VVVc)=NULL\n");
        }

        if (_EV_(VVVs) != NULL) {
            printf("*_EV_(VVVs)=%d\n", *_EV_(VVVs));
        } else {
            printf("_EV_(VVVs)=NULL\n");
        }

        
        if (_EV_(VVVst_Girl) != NULL) {
            printf("My name is %s.\nMy age is %u.\nI'm a %s.\n",
                _EV_(VVVst_Girl)->name,
                _EV_(VVVst_Girl)->age,
                _EV_(VVVst_Girl)->sex ? "boy" : "girl"
            );
        } else {
            printf("_EV_(VVVst_Girl)=NULL\n");
        }
    }

    printf("----------------------------------------------\n");

    //exit(0);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
int nRelocationRefVar = 0;
} // extern "C" {

static BOOL g_bFirstProcessAttach = TRUE;
void OnProcessAttach()
{
    BOOL bSuc;

    if (g_bFirstProcessAttach) {
        g_bFirstProcessAttach = FALSE;

        printf("SendInjectionFinished(): &nRelocationRefVar=0x%08X\n", &nRelocationRefVar);

        HANDLE hThread = CreateThread(NULL, 0, &TestThread, NULL, 0, NULL);
        if (hThread == NULL) {
            LOG_LINE();
            return;
        }
        CloseHandle(hThread);

        LOG_LINE();
        bSuc = DestProcInfo::GetInstance().Map();
        if (bSuc) {
            LOG_LINE();
            DestProcInfo::GetInstance().GetPtr()->nRefVarAddrAfterRelocation = (unsigned int)&nRelocationRefVar;
            SendInjectionFinished();
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////
#pragma data_seg("hookdata")
HINSTANCE g_hInstance = NULL;
HHOOK g_hHook = NULL;
HWND g_hWndDest = NULL;
BOOL g_bHookTaskExcuted = FALSE;
#pragma data_seg()
#pragma comment(linker,"/SECTION:hookdata,RWS")

#define MONITORED_WINDOW_NAME       TEXT("HookedWindow")

extern int ExecuteCodeInExeFile();

#if defined(WIN32_X86) // for Win32 X86
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
#elif defined(WINCE_ARM32) // for WinCE ARM32
BOOL WINAPI DllMain(HANDLE hModule,     // handle to DLL module   /*DG C6MR-ISQPcpp Rule 5.1.1-MMP_RAD0066*/
                    DWORD ul_reason_for_call,      // reason for calling function
                    LPVOID lpReserved )  // reserved
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if (HOOK_METHOD == INJECT_METHON__DIRECTLY) || (HOOK_METHOD == INJECT_METHON__DIRECTLY_WITH_HOOK)
        OnProcessAttach();
#elif HOOK_METHOD == INJECT_METHON__BY_SETWINDOWSHOOKEX
        g_hInstance = (HINSTANCE)hModule;
#else
#error HOOK_METHOD is undefinded
#endif // #if 0
        break;
    case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    DBG_LOG(TEXT("[%s]%S(): enter\n"), EXE_PATH, __FUNCTION__);
    int nResult = ExecuteCodeInExeFile();
    DBG_LOG(TEXT("[%s]%S(): nResult=%d\n"), EXE_PATH, __FUNCTION__, nResult);
    DBG_LOG(TEXT("[%s]%S(): exit\n"), EXE_PATH, __FUNCTION__);
    return 0;
}

#if defined(WIN32_X86) // for Win32 X86
typedef HHOOK (WINAPI *SetWindowsHookEx_t)(
    int idHook,
    HOOKPROC lpfn,
    HINSTANCE hMod,
    DWORD dwThreadId
);
typedef BOOL (WINAPI *UnhookWindowsHookEx_t)(
    HHOOK hhk
);
static SetWindowsHookEx_t pSetWindowsHookEx = NULL;
static UnhookWindowsHookEx_t pUnhookWindowsHookEx = NULL;
static BOOL GetHookFuncPtr()
{
    static HMODULE hSysDll = NULL;

    LOG_POS();
    if (hSysDll == NULL) {
        LOG_POS();
        hSysDll = LoadLibrary(TEXT("user32.dll"));
        if (hSysDll == NULL) {
            LOG_POS();
            return FALSE;
        }
    }

    LOG_POS();
    if (pSetWindowsHookEx == NULL) {
        LOG_POS();
        pSetWindowsHookEx = (SetWindowsHookEx_t)GetProcAddress(hSysDll, "SetWindowsHookExW");
        if (pSetWindowsHookEx == NULL) {
            LOG_POS();
            return FALSE;
        }
    }

    LOG_POS();
    if (pUnhookWindowsHookEx == NULL) {
        LOG_POS();
        pUnhookWindowsHookEx = (UnhookWindowsHookEx_t)GetProcAddress(hSysDll, "UnhookWindowsHookEx");
        if (pUnhookWindowsHookEx == NULL) {
            LOG_POS();
            return FALSE;
        }
    }
    
    LOG_POS();
    return TRUE;
}
#elif defined(WINCE_ARM32) // for WinCE ARM32
#define WH_MSGFILTER        (-1)
#define WH_JOURNALRECORD    0
#define WH_JOURNALPLAYBACK  1
#define WH_KEYBOARD         2
#define WH_GETMESSAGE       3
#define WH_CALLWNDPROC      4
#define WH_CBT              5
#define WH_SYSMSGFILTER     6
#define WH_MOUSE            7
typedef LRESULT (CALLBACK* HOOKPROC)(int code, WPARAM wParam, LPARAM lParam);
//typedef HINSTANCE HHOOK;
typedef struct {
    LPARAM lParam;
    WPARAM wParam;
    UINT message;
    HWND hwnd;
} CWPSTRUCT, *PCWPSTRUCT;
typedef HHOOK (WINAPI *SetWindowsHookEx_t)(
    int idHook,
    HOOKPROC lpfn,
    HINSTANCE hMod,
    DWORD dwThreadId
);
typedef BOOL (WINAPI *UnhookWindowsHookEx_t)(
    HHOOK hhk
);
static SetWindowsHookEx_t pSetWindowsHookEx = NULL;
static UnhookWindowsHookEx_t pUnhookWindowsHookEx = NULL;
static BOOL GetHookFuncPtr()
{
    static HMODULE hSysDll = NULL;

    if (hSysDll == NULL) {
        hSysDll = LoadLibrary(TEXT("coredll.dll"));
        if (hSysDll == NULL) {
            return FALSE;
        }
    }

    if (pSetWindowsHookEx == NULL) {
        pSetWindowsHookEx = (SetWindowsHookEx_t)GetProcAddress(hSysDll, TEXT("SetWindowsHookExW"));
        if (pSetWindowsHookEx == NULL) {
            return FALSE;
        }
    }

    if (pUnhookWindowsHookEx == NULL) {
        pUnhookWindowsHookEx = (UnhookWindowsHookEx_t)GetProcAddress(hSysDll, TEXT("UnhookWindowsHookEx"));
        if (pUnhookWindowsHookEx == NULL) {
            return FALSE;
        }
    }
    
    return TRUE;
}
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

static const int g_nHookId = WH_CALLWNDPROC/*WH_CALLWNDPROC*/ /*WH_KEYBOARD*/;
static LRESULT CALLBACK CallWndHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    //DBG_LOG(TEXT("[%s]%S(): enter\n"), EXE_PATH, __FUNCTION__);

    if (nCode < 0) {
        return CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }

    if (nCode == HC_ACTION) {
        //LOG_POS();
        if (g_hWndDest == NULL) {
            //LOG_POS();
            g_hWndDest = FindWindow(NULL, MONITORED_WINDOW_NAME);
        }
        if (g_hWndDest != NULL) {
//            LOG_POS();
            switch (g_nHookId) {
            case WH_CALLWNDPROC:
                {
                    //LOG_POS();
                    CWPSTRUCT *pCMPS = (CWPSTRUCT *)lParam;
                    if (pCMPS->hwnd == g_hWndDest) {
                        //LOG_POS();
                        g_bHookTaskExcuted = TRUE;
                        OnProcessAttach();
                    }
                }
                break;
            case WH_KEYBOARD:
                {
//                    LOG_POS();
                    //printf("*****wParam=%u,lParam=0x%08X\n", wParam, lParam);
                    //DBG_LOG(TEXT("*****wParam=%u,lParam=0x%08X\n"), wParam, lParam);
                    if (wParam == VK_F2) {
                        LOG_POS();
                        g_bHookTaskExcuted = TRUE;
                        OnProcessAttach();
                    }
                }
                break;
            default:
                //LOG_POS();
                break;
            }
        }
    }
    
    //DBG_LOG(TEXT("[%s]%S(): exit\n"), EXE_PATH, __FUNCTION__);
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

extern "C" {

TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API HD_ERROR InstallHook(void)
{
    HD_ERROR nRet = HDE_OK;

    if (!GetHookFuncPtr()) {
        nRet = HDE_HOOK_FUNC_PTR_NULL;
        goto FAILED;
    }

    if (g_hInstance == NULL) {
        nRet = HDE_DLL_NOT_LOADED;
        goto FAILED;
    }

    if (g_hHook != NULL) {
        nRet = HDE_HOOK_ALREADY_INSTALLED;
        goto FAILED;
    }
    
    g_hHook = pSetWindowsHookEx(g_nHookId, (HOOKPROC)&CallWndHookProc, g_hInstance, 0);
    if (g_hHook == NULL) {
        nRet = HDE_FAILED_TO_INSTALL_HOOK;
        goto FAILED;
    }

    nRet = HDE_OK;

FAILED:
    DBG_LOG(TEXT("[%s]%S(): return value: %d\n"), EXE_PATH, __FUNCTION__, nRet);
    return nRet;
}

TESTLOADEXTERNALSYMBAL_DLLTOINJECT_API HD_ERROR DeinstallHook(void)
{
    HD_ERROR nRet = HDE_OK;
    BOOL bSuc;

    if (!GetHookFuncPtr()) {
        nRet = HDE_HOOK_FUNC_PTR_NULL;
        goto FAILED;
    }

    if (g_hInstance == NULL) {
        nRet = HDE_DLL_NOT_LOADED;
        goto FAILED;
    }

    if (g_hHook == NULL) {
        nRet = HDE_HOOK_NOT_INSTALLED;
        goto FAILED;
    }

    bSuc = pUnhookWindowsHookEx(g_hHook);
    if (!bSuc) {
        nRet = HDE_FAILED_TO_DEINSTALL_HOOK;
        goto FAILED;
    }
    g_hHook = NULL;

    nRet = HDE_OK;

FAILED:
    DBG_LOG(TEXT("[%s]%S(): return value: %d\n"), EXE_PATH, __FUNCTION__, nRet);
    return nRet;
}

} // extern "C" {
