// HookDllLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define HOOK_DLL_DEF_NAME                           TEXT("HookDll.dll")
#define HOOK_DLL_DEF_PATH                           HOOK_DLL_DEF_NAME

#define INSTALL_FUNC_NAME                           TEXT("InstallHook")
#define DEINSTALL_FUNC_NAME                         TEXT("DeinstallHook")

#define DBG_LOG                                     _tprintf

typedef int LoadUnloadFunc_t(void);


static void usage(const std::tstring &argv0)
{
    _tprintf(TEXT("Usage:\n"));
    _tprintf(TEXT("    %s [load|unload] [<sHookDllPath>]\n"), argv0.c_str());
    _tprintf(TEXT("\n"));
}


int _tmain(int argc, _TCHAR* argv[])
{
    int nRet = 0;
    HMODULE hModule = NULL;
    std::tstring sHookDllPath = HOOK_DLL_DEF_PATH;
    bool bLoad = true;
    int nFuncRet;

    if (argc >= 2) {
        if (std::tstring(argv[1]) == TEXT("-H") || std::tstring(argv[1]) == TEXT("--help")) {
            usage(argv[0]);
            return 0;
        }
    }

    if (argc >= 2) {
        if (std::tstring(argv[1]) == TEXT("load")) {
            bLoad = true;
        } else if (std::tstring(argv[1]) == TEXT("unload")) {
            bLoad = false;
        } else {
            usage(argv[0]);
            return -1;
        }
    }

    if (argc >= 3) {
        if (std::tstring(argv[2]) != TEXT("")) {
            sHookDllPath = std::tstring(argv[2]);
        }
    }

    hModule = LoadLibraryEx(sHookDllPath.c_str(), NULL, 0);
    if (hModule == NULL) {
        nRet = -1;
        goto FAILED;
    }

    if (bLoad) {
#if defined(WIN32_X86) // for Win32 X86
        LoadUnloadFunc_t *pFunc = (LoadUnloadFunc_t *)GetProcAddress(hModule, tstr2str(INSTALL_FUNC_NAME).c_str());
#elif defined(WINCE_ARM32) // for WinCE ARM32
        LoadUnloadFunc_t *pFunc = (LoadUnloadFunc_t *)GetProcAddress(hModule, INSTALL_FUNC_NAME);
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
        if (pFunc != NULL) {
            nFuncRet = pFunc();
            DBG_LOG(TEXT("%S(): return of %s(): %d\n"), __FUNCTION__, INSTALL_FUNC_NAME, nFuncRet);
            _tprintf(TEXT("Enter any key to exit..."));
            getchar();
        } else {
            DBG_LOG(TEXT("%S(): getting address of %s from DLL \"%s\" is failed!\n"), __FUNCTION__, INSTALL_FUNC_NAME, sHookDllPath.c_str());
            nRet = -1;
            goto FAILED;
        }
    } else  {
#if defined(WIN32_X86) // for Win32 X86
        LoadUnloadFunc_t *pFunc = (LoadUnloadFunc_t *)GetProcAddress(hModule, tstr2str(DEINSTALL_FUNC_NAME).c_str());
#elif defined(WINCE_ARM32) // for WinCE ARM32
        LoadUnloadFunc_t *pFunc = (LoadUnloadFunc_t *)GetProcAddress(hModule, DEINSTALL_FUNC_NAME);
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)
        if (pFunc != NULL) {
            nFuncRet = pFunc();
            DBG_LOG(TEXT("%S(): return of %s(): %d\n"), __FUNCTION__, DEINSTALL_FUNC_NAME, nFuncRet);
        } else {
            DBG_LOG(TEXT("%S(): getting address of %s from DLL \"%s\" is failed!\n"), __FUNCTION__, DEINSTALL_FUNC_NAME, sHookDllPath.c_str());
            nRet = -1;
            goto FAILED;
        }
    }

    nRet = 0;

FAILED:
    if (hModule != NULL) {
        FreeLibrary(hModule);
        hModule = NULL;
    }

	return nRet;
}

