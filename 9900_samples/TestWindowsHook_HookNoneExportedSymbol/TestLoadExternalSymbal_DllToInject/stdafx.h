// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "Config.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

#if defined(WIN32_X86) // for Win32 X86
#define LOG_FILE_PATH                       "d:\\HookDll.log"
#elif defined(WINCE_ARM32) // for WinCE ARM32
#define LOG_FILE_PATH                       "\\release\\HookDll.log"
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)


#define EXE_PATH                            GetModulePath().c_str()
#define DBG_LOG                             DbgPrintf

#if 0
#define LOG_POS() \
    do { \
        char buf[1024]; \
        sprintf(buf, "echo %s:%d>>%s", __FILE__, __LINE__, LOG_FILE_PATH); \
        system(buf); \
    } while(0)
#else
#define LOG_POS()
#endif

#if defined(UNICODE) || defined(_UNICODE)
    namespace std {
        typedef wstring tstring;
    } // namespace std {
    typedef wchar_t tchar_t;
#else
    namespace std {
        typedef string tstring;
    } // namespace std {
    typedef char tchar_t;
#endif // #if defined(UNICODE) || defined(_UNICODE)

// TODO: reference additional headers your program requires here
int DbgPrintf(TCHAR *format, ...);
std::tstring GetModulePath();

std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);

std::string tstr2str(const std::tstring &tstrInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
std::tstring str2tstr(const std::string &strInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);

