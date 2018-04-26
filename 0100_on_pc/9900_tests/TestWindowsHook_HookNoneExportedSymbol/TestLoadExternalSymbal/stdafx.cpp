// stdafx.cpp : source file that includes just the standard includes
// TestLoadExternalSymbal.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <windows.h>
#include <list>
#include <map>
#include <assert.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#if defined(WIN32_X86) // for Win32 X86
#elif defined(WINCE_ARM32) // for WinCE ARM32
#else
#error the platform is not supported!
#endif // #if defined(WIN32_X86)

#define TK_ASSERT(statement)    assert(statement)

template<typename T>
class AutoFreeArrayPtr {
public:
    AutoFreeArrayPtr(T *point = NULL) : m_point(point) {}
    ~AutoFreeArrayPtr() {
        delete [] m_point;
    }
    void Set(T *point) { m_point = point; }
private:
    T *m_point;
};

template<typename T>
class AutoFreePtr {
public:
    AutoFreePtr(T *point = NULL) : m_point(point) {}
    ~AutoFreePtr() {
        delete m_point;
    }
    void Set(T *point) { m_point = point; }
private:
    T *m_point;
};

std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage)
{
    std::string strOutput;
    AutoFreeArrayPtr<char> afapBuf;
    char *pBuf;
    unsigned nBufSize;
    int nRet;
    DWORD nLastError = 0;

    nRet = ::WideCharToMultiByte(
        nCodePage,
        0,
        wstrInput.c_str(),
        wstrInput.length(),
        NULL,
        0,
        "?",
        NULL
    );
    if (nRet <= 0)
    {
        nLastError = ::GetLastError();
        goto FAILED;
    }

    nBufSize = nRet + 1;
    pBuf = new char[nBufSize];
    if (pBuf == NULL) {
        TK_ASSERT(0);
        goto FAILED;
    }
    afapBuf.Set(pBuf);

    nRet = ::WideCharToMultiByte(
        nCodePage,
        0,
        wstrInput.c_str(),
        wstrInput.length(),
        pBuf,
        nBufSize,
        "?",
        NULL
    );
    if (nRet <= 0)
    {
        nLastError = ::GetLastError();
        goto FAILED;
    }
    pBuf[nRet] = '\0';

    strOutput = pBuf;
    
FAILED:
    return strOutput;
}

std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage)
{
    std::wstring wstrOutput;
    AutoFreeArrayPtr<wchar_t> afapBuf;
    wchar_t *pBuf;
    unsigned nBufSize;
    int nRet;
    DWORD nLastError = 0;

    nRet = ::MultiByteToWideChar(
        nCodePage,
        0,
        strInput.c_str(),
        strInput.length(),
        NULL,
        0
    );
    if (nRet <= 0)
    {
        nLastError = ::GetLastError();
        goto FAILED;
    }

    nBufSize = nRet + 1;
    pBuf = new wchar_t[nBufSize];
    if (pBuf == NULL) {
        TK_ASSERT(0);
        goto FAILED;
    }
    afapBuf.Set(pBuf);

    nRet = ::MultiByteToWideChar(
        nCodePage,
        0,
        strInput.c_str(),
        strInput.length(),
        pBuf,
        nBufSize
    );
    if (nRet <= 0)
    {
        nLastError = ::GetLastError();
        goto FAILED;
    }
    pBuf[nRet] = L'\0';

    wstrOutput = pBuf;
    
FAILED:
    return wstrOutput;
}

std::string tstr2str(const std::tstring &tstrInput, unsigned long nCodePage)
{
#if defined(UNICODE) || defined(_UNICODE)
        return wstr2str(tstrInput, nCodePage);
#else
        return tstrInput;
#endif
}

std::tstring str2tstr(const std::string &strInput, unsigned long nCodePage)
{
#if defined(UNICODE) || defined(_UNICODE)
        return str2wstr(strInput, nCodePage);
#else
        return strInput;
#endif
}
