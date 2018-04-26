// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "Config.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

// TODO: reference additional headers your program requires here

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

std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);

std::string tstr2str(const std::tstring &tstrInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
std::tstring str2tstr(const std::string &strInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);