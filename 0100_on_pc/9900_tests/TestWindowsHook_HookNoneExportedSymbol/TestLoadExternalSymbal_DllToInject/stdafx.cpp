// stdafx.cpp : source file that includes just the standard includes
// TestLoadExternalSymbal_DllToInject.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string>

static int OutputToLogFile(const std::tstring &sContent)
{
    int nRet = 0;
    FILE *pFile = NULL;
    size_t nWritten;

    pFile = fopen(LOG_FILE_PATH, "a+b");
    if (pFile == NULL) {
        nRet = -1;
        goto FAILED;
    }

    nWritten = fwrite(tstr2str(sContent).c_str(), 1, sContent.length(), pFile);
    if (nWritten != sContent.length()) {
        nRet = -1;
        goto FAILED;
    }

    nRet = 0;

FAILED:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return nRet;
}

int DbgPrintf(TCHAR *format, ...)
{
    va_list args;
    int len = 0;
    TCHAR *buffer = NULL;
    
    if (format == NULL) {
        return 0;
    }

    // retrieve the variable arguments
    va_start(args, format);
    
    len = _vsctprintf(format, args)   // _vscprintf doesn't count
                                + 1; // terminating '\0'
    
    buffer = (TCHAR *)malloc(len * sizeof(TCHAR));
    if (buffer == NULL) {
       return 0;
    }

    _vstprintf(buffer, format, args); // C4996
    // Note: vsprintf is deprecated; consider using vsprintf_s instead
    
    //OutputDebugStringA(buffer);
    OutputToLogFile(buffer);

    va_end(args);
    free(buffer);
    
    return len;
}

std::tstring GetModulePath()
{
    std::tstring sResult;
    TCHAR buf[1024];
    DWORD nSize;

    nSize = GetModuleFileName(NULL, buf, sizeof(buf) / sizeof(TCHAR) - 1);
    if (nSize > 0) {
        sResult = buf;
    }

    return sResult;
}

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
