#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include "TK_Tools.h"

#ifdef WIN32
#define unlink _unlink
#endif // WIN32

namespace TK_Tools {

#ifdef WIN32
std::string FormatStr(const char *pFormat, ...)
{
    std::string strRet;
    va_list args;
    int len;
    char *buffer;

    va_start(args, pFormat);                //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001
    len = _vscprintf(pFormat, args) + 1;    // NOTE: _vscprintf doesn't count terminating '\0'
    buffer = static_cast<char*>(malloc(len * sizeof(char)));
    if (buffer != NULL) {
        vsprintf(buffer, pFormat, args);
        strRet = buffer;
        free(buffer);
    } else {
        strRet = "";
    }
    va_end(args);                           //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001

    return strRet;
}
#else
std::string FormatStr(const char *pFormat, ...)
{
    std::string strRet;
    va_list args;
    int len;
    char *buffer = NULL;

    va_start(args, pFormat);                //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001
    len = vasprintf(&buffer, pFormat, args);
    if (len >= 0) {
        strRet = buffer;
        free(buffer);
    }
    va_end(args);                           //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001

    return strRet;
}
#endif // #ifdef WIN32

#ifdef WIN32
void FormatStr(std::string &strRet, const char *pFormat, ...)
{
    va_list args;
    int len;
    char *buffer;

    va_start(args, pFormat);                //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001
    len = _vscprintf(pFormat, args) + 1;    // NOTE: _vscprintf doesn't count terminating '\0'
    buffer = static_cast<char*>(malloc(len * sizeof(char)));
    if (buffer != NULL) {
        vsprintf(buffer, pFormat, args);
        strRet = buffer;
        free(buffer);
    } else {
        strRet = "";
    }
    va_end(args);                           //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001
}
#else
void FormatStr(std::string &strRet, const char *pFormat, ...)
{
    va_list args;
    int len;
    char *buffer = NULL;

    va_start(args, pFormat);                //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001
    len = vasprintf(&buffer, pFormat, args);
    if (len >= 0) {
        strRet = buffer;
        free(buffer);
    }
    va_end(args);                           //DG CPP2ISQP-ISQPcpp Rule 5.5.9-MMP_ExtGen_0001
}
#endif // #ifdef WIN32

#ifdef WIN32
std::wstring FormatStr(const wchar_t *pFormat, ...)
{
    std::wstring strRet;
    va_list args;
    int len;
    wchar_t *buffer;

    va_start(args, pFormat);
    len = _vscwprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
    buffer = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (buffer != NULL) {
        wvsprintf(buffer, pFormat, args);
        strRet = buffer;
        free(buffer);
    } else {
        strRet = L"";
    }
    va_end(args);

    return strRet;
}
#endif // #ifdef WIN32

#ifdef WIN32
void FormatStr(std::wstring &strRet, const wchar_t *pFormat, ...)
{
    va_list args;
    int len;
    wchar_t *buffer;

    va_start(args, pFormat);
    len = _vscwprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
    buffer = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (buffer != NULL) {
        wvsprintf(buffer, pFormat, args);
        strRet = buffer;
        free(buffer);
    } else {
        strRet = L"";
    }
    va_end(args);
}
#endif // #ifdef WIN32

std::string TrimLeft(const std::string &sText, const std::string &sTrimChars /*= " \t\r\n"*/)
{
    std::string sResult;
    const char *p = sText.c_str();
    for (; *p != '\0'; p++) {
        std::string::size_type nPos = sTrimChars.find(*p);
        if (nPos == std::string::npos) {
            break;
        }
    }

    sResult = p;
    return sResult;
}

std::wstring TrimLeft(const std::wstring &sText, const std::wstring &sTrimChars /*= L" \t\r\n"*/)
{
    std::wstring sResult;
    const wchar_t *p = sText.c_str();
    for (; *p != L'\0'; p++) {
        std::wstring::size_type nPos = sTrimChars.find(*p);
        if (nPos == std::wstring::npos) {
            break;
        }
    }

    sResult = p;
    return sResult;
}

std::string TrimRight(const std::string &sText, const std::string &sTrimChars /*= " \t\r\n"*/)
{
    std::string sResult;

    if (sText.length() != 0) {
        const char *pFirst = sText.c_str();
        const char *p = pFirst + sText.length() - 1;
        for (; p >= pFirst; p--) {
            std::string::size_type nPos = sTrimChars.find(*p);
            if (nPos == std::string::npos) {
                break;
            }
        }
        p++;

        sResult = sText.substr(0, p - pFirst);
    }

    return sResult;
}

std::wstring TrimRight(const std::wstring &sText, const std::wstring &sTrimChars /*= L" \t\r\n"*/)
{
    std::wstring sResult;

    if (sText.length() != 0) {
        const wchar_t *pFirst = sText.c_str();
        const wchar_t *p = pFirst + sText.length() - 1;
        for (; p >= pFirst; p--) {
            std::wstring::size_type nPos = sTrimChars.find(*p);
            if (nPos == std::wstring::npos) {
                break;
            }
        }
        p++;

        sResult = sText.substr(0, p - pFirst);
    }

    return sResult;
}

std::string TrimAll(const std::string &sText, const std::string &sTrimChars /*= " \t\r\n"*/)
{
    std::string sResult;

    sResult = TrimLeft(sText, sTrimChars);
    sResult = TrimRight(sResult, sTrimChars);

    return sResult;
}

std::wstring TrimAll(const std::wstring &sText, const std::wstring &sTrimChars /*= L" \t\r\n"*/)
{
    std::wstring sResult;

    sResult = TrimLeft(sText, sTrimChars);
    sResult = TrimRight(sResult, sTrimChars);

    return sResult;
}

std::vector<std::string> SplitString(const std::string &sText, const std::string &sSpaceChars /*= " \t\r\n"*/)
{
    std::vector<std::string> vResult;

    const char *p = sText.c_str();
    std::string sTemp;
    for (; *p != '\0'; p++) {
        std::string::size_type nPos = sSpaceChars.find(*p);
        if (nPos == std::string::npos) {                // non space char
            sTemp += *p;
        } else {                                        // space char
            if (sTemp.length() != 0) {
                vResult.push_back(sTemp);
                sTemp = "";
            }
        }
    }
    if (sTemp != "") {
        vResult.push_back(sTemp);
        sTemp = "";
    }

    return vResult;
}

std::vector<std::wstring> SplitString(const std::wstring &sText, const std::wstring &sSpaceChars /*= L" \t\r\n*/)
{
    std::vector<std::wstring> vResult;

    const wchar_t *p = sText.c_str();
    std::wstring sTemp;
    for (; *p != '\0'; p++) {
        std::wstring::size_type nPos = sSpaceChars.find(*p);
        if (nPos == std::wstring::npos) {                // non space char
            sTemp += *p;
        } else {                                        // space char
            if (sTemp.length() != 0) {
                vResult.push_back(sTemp);
                sTemp = L"";
            }
        }
    }
    if (sTemp != L"") {
        vResult.push_back(sTemp);
        sTemp = L"";
    }

    return vResult;
}

#ifdef WIN32
std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage)
{
    int nError = 0;
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
        nError = -1;
    }

    if (nError == 0) {
        nBufSize = nRet + 1;
        pBuf = new char[nBufSize];
        if (pBuf == NULL) {
            TK_ASSERT(0);
            nError = -1;
        }
    }
    if (nError == 0) {
        afapBuf.Set(pBuf);
    }

    if (nError == 0) {
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
            nError = -1;
        }
    }
    if (nError == 0) {
        if (pBuf != NULL) {
            pBuf[nRet] = '\0';
        }

        strOutput = pBuf;
    }

    return strOutput;
}
#endif // #ifdef WIN32

#ifdef WIN32
std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage)
{
    int nError = 0;
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
        nError = -1;
    }

    if (nError == 0) {
        nBufSize = nRet + 1;
        pBuf = new wchar_t[nBufSize];
        if (pBuf == NULL) {
            TK_ASSERT(0);
            nError = -1;
        }
    }
    if (nError == 0) {
        afapBuf.Set(pBuf);
    }

    if (nError == 0) {
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
            nError = -1;
        }
    }
    if (nError == 0) {
        if (pBuf != NULL) {
            pBuf[nRet] = L'\0';
        }

        wstrOutput = pBuf;
    }

    return wstrOutput;
}
#endif // #ifdef WIN32

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

std::string UpperCase(const std::string &sText)
{
    std::string sResult;

    for (std::string::size_type i = 0; i < sText.length(); i++) {
        char ch = sText[i];
        if ('a' <= ch && ch <= 'z') {
            char ch2 = ch + 'A' - 'a';
            sResult += ch2;
        } else {
            sResult += ch;
        }
    }

    return sResult;
}

std::string LowerCase(const std::string &sText)
{
    std::string sResult;

    for (std::string::size_type i = 0; i < sText.length(); i++) {
        char ch = sText[i];
        if ('A' <= ch && ch <= 'Z') {
            char ch2 = ch + 'a' - 'A';
            sResult += ch2;
        } else {
            sResult += ch;
        }
    }

    return sResult;
}


int CompareCase(const std::string &sText1, const std::string &sText2)
{
    return strcmp(sText1.c_str(), sText2.c_str());
}

int CompareCaseLeftLen(const std::string &sText1, const std::string &sText2)
{
    std::string sText2Trunced = sText2.substr(0, sText1.length());
    return strcmp(sText1.c_str(), sText2Trunced.c_str());
}

int CompareCaseLeftLen(const std::wstring &sText1, const std::wstring &sText2)
{
    std::wstring sText2Trunced = sText2.substr(0, sText1.length());
    return wcscmp(sText1.c_str(), sText2Trunced.c_str());
}

int CompareCaseRightLen(const std::string &sText1, const std::string &sText2)
{
    std::string sText1Trunced = sText1.substr(0, sText2.length());
    return strcmp(sText1Trunced.c_str(), sText2.c_str());
}

int CompareCaseMinLen(const std::string &sText1, const std::string &sText2)
{
    if (sText1.length() >= sText2.length()) {
        return CompareCaseRightLen(sText1, sText2);
    } else {
        return CompareCaseLeftLen(sText1, sText2);
    }
}

int CompareNoCase(const std::string &sText1, const std::string &sText2)
{
    return strcmp(UpperCase(sText1).c_str(), UpperCase(sText2).c_str());
}

int CompareNoCaseLeftLen(const std::string &sText1, const std::string &sText2)
{
    std::string sText2Trunced = sText2.substr(0, sText1.length());
    return strcmp(UpperCase(sText1).c_str(), UpperCase(sText2Trunced).c_str());
}

int CompareNoCaseRightLen(const std::string &sText1, const std::string &sText2)
{
    std::string sText1Trunced = sText1.substr(0, sText2.length());
    return strcmp(UpperCase(sText1Trunced).c_str(), UpperCase(sText2).c_str());
}

int CompareNoCaseMinLen(const std::string &sText1, const std::string &sText2)
{
    if (sText1.length() >= sText2.length()) {
        return CompareNoCaseRightLen(sText1, sText2);
    } else {
        return CompareNoCaseLeftLen(sText1, sText2);
    }
}

unsigned long StrToUL(const std::string &sNumber, int nBase /*= 0*/)
{
    unsigned long nNumber = 0;

    nNumber = strtoul(sNumber.c_str(), NULL, nBase);
    return nNumber;
}

unsigned long StrToUL(const std::wstring &sNumber, int nBase /*= 0*/)
{
    unsigned long nNumber = 0;

    nNumber = wcstoul(sNumber.c_str(), NULL, nBase);
    return nNumber;
}

long StrToL(const std::string &sNumber, int nBase /*= 0*/)
{
    long nNumber = 0;

    nNumber = strtol(sNumber.c_str(), NULL, nBase);
    return nNumber;
}

long StrToL(const std::wstring &sNumber, int nBase /*= 0*/)
{
    long nNumber = 0;

    nNumber = wcstol(sNumber.c_str(), NULL, nBase);
    return nNumber;
}

float StrToFloat(const std::string &sNumber)
{
    float fResult = 0.0f;

    sscanf(sNumber.c_str(), "%f", &fResult);
    return fResult;
}

float StrToFloat(const std::wstring &sNumber)
{
    float fResult = 0.0f;

    swscanf(sNumber.c_str(), L"%f", &fResult);
    return fResult;
}

bool LoadFile(const std::string &sFilePath, std::string &sFileContent)
{
    int nError = 0;
    bool bRet = false;
    FILE *pFile = NULL;
    long nFileSize = 0;
    char *pFileContentBuf = NULL;
    AutoFreeArrayPtr<char> afap;
    size_t nRead;

    if (nError == 0) {
        pFile = fopen(sFilePath.c_str(), "rb");
        if (pFile == NULL) {
            bRet = false;
            nError = -1;
        }
    }
    if (nError == 0) {
        fseek(pFile, 0, SEEK_END);
        nFileSize = ftell(pFile);
        pFileContentBuf = new char[nFileSize + 1];
        TK_ASSERT(pFileContentBuf != NULL);
        pFileContentBuf[nFileSize] = '\0';
        afap.Set(pFileContentBuf);

        fseek(pFile, 0, SEEK_SET);
        nRead = fread(pFileContentBuf, 1, nFileSize, pFile);
        if (static_cast<signed int>(nRead) != nFileSize) {
            bRet = false;
            nError = -1;
        }
    }

    if (nError == 0) {
        sFileContent = pFileContentBuf;
        bRet = true;
    }

    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return bRet;
}

bool SaveToFile(const std::string &sFilePath, std::string &sFileContent)
{
    bool bRet = true;
    FILE *pFile = NULL;
    size_t nWrite;

    if (bRet) {
        pFile = fopen(sFilePath.c_str(), "w+b");
        if (pFile == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        nWrite = fwrite(sFileContent.c_str(), 1, sFileContent.size(), pFile);
        if (nWrite != sFileContent.size()) {
            bRet = false;
        }
    }

    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return bRet;
}

bool FileExists(const std::string &sFilePath)
{
    bool bRet;
    FILE *pFile;

    pFile = fopen(sFilePath.c_str(), "rb");
    if (pFile != NULL) {
        fclose(pFile);
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool CreateFile(const std::string &sFilePath)
{
    bool bSuc;
    std::string sContent("");

    bSuc = SaveToFile(sFilePath, sContent);

    return bSuc;
}

bool DeleteFile(const std::string &sFilePath)
{
#ifndef WINCE
    bool bRet = true;
    int nRet;

    if (bRet) {
        nRet = ::unlink(sFilePath.c_str());
        if (nRet != 0) {
            bRet = false;
        }
    }

    return bRet;
#else
    return false;
#endif
}

void Sleep(unsigned int nMiliSecond)
{
#ifdef WIN32
    ::Sleep(nMiliSecond);
#else
    int nRet;
    struct timeval tv;

    tv.tv_sec = nMiliSecond / 1000;
    tv.tv_usec = nMiliSecond % 1000 * 1000;
    do {
        nRet = ::select(0, NULL, NULL, NULL, &tv);
        if (nRet == -1 && errno == EINTR) {
            continue;
        }
    } while (false);
    //::usleep(nMiliSecond * 1000);
#endif // #ifdef WIN32
}

static std::string GetObjectFileFullPath(const std::string &sFilePath)
{
    return "";
}

static bool IncRefOfObjectFile(const std::string &sFilePath)
{
    bool bSuc = true;
    std::string sFileContent;
    std::string sFileContentNew;
    long nRef = 0;

    if (bSuc) {
        bSuc = LoadFile(GetObjectFileFullPath(sFilePath), sFileContent);
    }

    if (bSuc) {
        sFileContent = TrimAll(sFileContent);
        if (!sFileContent.empty()) {
            nRef = StrToL(sFileContent);
            if (nRef < 0) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        nRef++;
        sFileContentNew = FormatStr("%ld\n", nRef);
        bSuc = SaveToFile(GetObjectFileFullPath(sFilePath), sFileContentNew);
    }

    return bSuc;
}

static bool DecRefOfObjectFile(const std::string &sFilePath)
{
    bool bSuc = true;
    std::string sFileContent;
    std::string sFileContentNew;
    long nRef = 0;

    if (bSuc) {
        bSuc = LoadFile(GetObjectFileFullPath(sFilePath), sFileContent);
    }

    if (bSuc) {
        sFileContent = TrimAll(sFileContent);
        if (sFileContent.empty()) {
            bSuc = false;
        } else {
            nRef = StrToL(sFileContent);
            if (nRef <= 0) {
                bSuc = false;
            }
        }
    }

    if (bSuc) {
        nRef--;
        sFileContentNew = FormatStr("%ld\n", nRef);
        bSuc = SaveToFile(GetObjectFileFullPath(sFilePath), sFileContentNew);
    }

    return bSuc;
}

static bool GetRefOfObjectFile(const std::string &sFilePath, long &nRef)
{
    bool bSuc = true;
    std::string sFileContent;
    std::string sFileContentNew;

    if (bSuc) {
        bSuc = LoadFile(GetObjectFileFullPath(sFilePath), sFileContent);
    }

    if (bSuc) {
        sFileContent = TrimAll(sFileContent);
        nRef = StrToL(sFileContent);
        if (nRef < 0) {
            bSuc = false;
        }
    }

    return bSuc;
}

bool RetainObjectFile(const std::string &sFilePath)
{
    bool bSuc = true;

    if (bSuc) {
        if (!ObjectFileExists(sFilePath)) {
            bSuc = CreateFile(GetObjectFileFullPath(sFilePath));
        }
    }

    if (bSuc) {
        bSuc = IncRefOfObjectFile(sFilePath);
    }

    return bSuc;
}

bool ReleaseObjectFile(const std::string &sFilePath)
{
    bool bSuc = true;
    long nRef = 1;

    if (bSuc) {
        bSuc = ObjectFileExists(sFilePath);
    }

    if (bSuc) {
        bSuc = DecRefOfObjectFile(sFilePath);
    }

    if (bSuc) {
        bSuc = GetRefOfObjectFile(sFilePath, nRef);
    }

    if (bSuc) {
        if (nRef <= 0) {
            bSuc = DeleteFile(GetObjectFileFullPath(sFilePath));
        }
    }

    return bSuc;
}

bool ObjectFileExists(const std::string &sFilePath)
{
    bool bExists;

    bExists = FileExists(GetObjectFileFullPath(sFilePath));

    return bExists;
}

#ifndef WIN32
bool ProcessExists(const std::string &sExeName)
{
    int nError = 0;
    bool bExists = false;
    FILE *pFile = NULL;
    long nFileSize = 0;
    char *pFileContentBuf = NULL;
    AutoFreeArrayPtr<char> afap;
    size_t nRead;
#ifdef __CYGWIN__
    std::string sCmdLine = FormatStr("ps aux -W | grep -E -e \"%s$\" | grep -v \"grep\" | wc -l", sExeName.c_str());
#else
    std::string sCmdLine = FormatStr("ps aux | grep -E -e \"%s$\" | grep -v \"grep\" | wc -l", sExeName.c_str());
#endif // #ifdef __CYGWIN__
    std::string sFileContent;

    if (nError == 0) {
        pFile = popen(sCmdLine.c_str(), "r");
        if (pFile == NULL) {
            nError = -1;
        }
    }
    if (nError == 0) {
        nFileSize = 100;
        pFileContentBuf = new char[nFileSize + 1];
        TK_ASSERT(pFileContentBuf != NULL);
        memset(pFileContentBuf, 0, nFileSize + 1);
        afap.Set(pFileContentBuf);

        nRead = fread(pFileContentBuf, 1, nFileSize, pFile);
        if (nRead <= 0) {
            nError = -1;
        }
    }

    if (nError == 0) {
        sFileContent = pFileContentBuf;
        sFileContent = TrimAll(sFileContent);
    }

    if (pFile != NULL) {
        pclose(pFile);
        pFile = NULL;
    }

    if (StrToUL(sFileContent) >= 1) {
        bExists = true;
    }

    return bExists;
}
#endif // #ifndef WIN32

#ifndef WIN32
bool GetFileINode(const std::string &sFilePath, std::string &sINode)
{
    bool bRet = true;
    int nRet;
    struct stat oState;

    if (bRet) {
        bRet = FileExists(sFilePath);
        if (!bRet) {
            bRet = CreateFile(sFilePath);
        }
    }

    if (bRet) {
        nRet = ::lstat(sFilePath.c_str(), &oState);
        if (nRet != 0) {
            bRet = false;
        }
    }

    if (bRet) {
        sINode = FormatStr("%ld", oState.st_ino);
    }

    return bRet;
}
#endif // #ifndef WIN32

#ifndef WIN32
#ifndef OSX64
bool GetAbsoluteTimeSpec(unsigned int nMillisecondShift, struct timespec &ts, clockid_t nClockId /*= CLOCK_REALTIME*/)
{
    bool bRet = true;
    int nRet;

    memset(&ts, 0, sizeof(struct timespec));

    if (bRet) {
        nRet = clock_gettime(nClockId, &ts);
        if (nRet != 0) {
            bRet = false;
        }
    }

    if (bRet) {
        ts.tv_sec += nMillisecondShift / 1000;
        ts.tv_nsec += nMillisecondShift % 1000 * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec += 1;
        }
    }

    return bRet;
}
#endif // #ifndef OSX64
#endif // #ifndef WIN32

std::string GetCurrentProcessUIDStr()
{
    std::string sProcessUIDStr;

#ifdef WIN32
    FormatStr(sProcessUIDStr, "%09lu", ::GetCurrentProcessId());
#else
    FormatStr(sProcessUIDStr, "%09%lu", ::getpid());
#endif // WIN32

    return sProcessUIDStr;
}

bool IsAlphabetChar(char nChar)
{
    bool bRet = false;

    if ('A' <= nChar && nChar <= 'Z') {
        bRet = true;
    } else if ('a' <= nChar && nChar <= 'z') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsAlphabetChar(wchar_t nChar)
{
    bool bRet = false;

    if (L'A' <= nChar && nChar <= L'Z') {
        bRet = true;
    } else if (L'a' <= nChar && nChar <= L'z') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsDigitChar(char nChar)
{
    bool bRet = false;

    if ('0' <= nChar && nChar <= '9') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsDigitChar(wchar_t nChar)
{
    bool bRet = false;

    if (L'0' <= nChar && nChar <= L'9') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsHexDigitChar(char nChar)
{
    bool bRet = false;

    if ('0' <= nChar && nChar <= '9') {
        bRet = true;
    } else if ('A' <= nChar && nChar <= 'F') {
        bRet = true;
    } else if ('a' <= nChar && nChar <= 'f') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsHexDigitChar(wchar_t nChar)
{
    bool bRet = false;

    if (L'0' <= nChar && nChar <= L'9') {
        bRet = true;
    } else if (L'A' <= nChar && nChar <= L'F') {
        bRet = true;
    } else if (L'a' <= nChar && nChar <= L'f') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsHexDigitString(const std::string &s)
{
    bool bRet = true;
    unsigned int i;

    for (i = 0; i < s.length(); ++i) {
        if (!IsHexDigitChar(s[i])) {
            bRet = false;
            break;
        }
    }

    return bRet;
}

bool IsHexDigitString(const std::wstring &s)
{
    bool bRet = true;
    unsigned int i;

    for (i = 0; i < s.length(); ++i) {
        if (!IsHexDigitChar(s[i])) {
            bRet = false;
            break;
        }
    }

    return bRet;
}

bool IsPinyinStr(const std::string &sPinyin)
{
    bool bRet = true;
    char letter;
    char tone;
    unsigned int i;

    if (sPinyin.length() < 2) {
        bRet = false;
    } else {
        tone = sPinyin[sPinyin.length() - 1];
        if (tone < '1' || tone > '5') {
            bRet = false;
        } else {
            for (i = 0; i < sPinyin.length() - 2; ++i) {
                letter = sPinyin[i];
                if (letter < 'a' || letter > 'z') {
                    bRet = false;
                }
            }
        }
    }

    return bRet;
}

bool IsPinyinStr(const std::wstring &sPinyin)
{
    bool bRet = true;
    wchar_t letter;
    wchar_t tone;
    unsigned int i;

    if (sPinyin.length() < 2) {
        bRet = false;
    } else {
        tone = sPinyin[sPinyin.length() - 1];
        if (tone < L'1' || tone > L'5') {
            bRet = false;
        } else {
            for (i = 0; i < sPinyin.length() - 2; ++i) {
                letter = sPinyin[i];
                if (letter < L'a' || letter > L'z') {
                    bRet = false;
                }
            }
        }
    }

    return bRet;
}

bool GetPinyinInfo(const std::string &sPinyin, std::string &sPinyinWithTone, unsigned int &tone)
{
    bool bRet = true;

    if (bRet) {
        bRet = IsPinyinStr(sPinyin);
    }

    if (bRet) {
        sPinyinWithTone = sPinyin.substr(0, sPinyin.length() - 1);
        tone = StrToUL(sPinyin.substr(sPinyin.length() - 1, 1));
    }

    return bRet;
}

bool GetPinyinInfo(const std::wstring &sPinyin, std::wstring &sPinyinWithTone, unsigned int &tone)
{
    bool bRet = true;

    if (bRet) {
        bRet = IsPinyinStr(sPinyin);
    }

    if (bRet) {
        sPinyinWithTone = sPinyin.substr(0, sPinyin.length() - 1);
        tone = sPinyin[sPinyin.length() - 1] - L'0';
    }

    return bRet;
}

bool GetSignedIntFromStr(const std::string &sNumberStr, int *pRetSignedInt /*= NULL*/)
{
    bool bRet = true;
    std::string sNumberStrTemp = sNumberStr;
    bool bNegative = false;
    unsigned int i;

    if (pRetSignedInt != NULL) {
        *pRetSignedInt = 0;
    }

    if (bRet) {
        if (sNumberStrTemp.empty()) {
            bRet = false;
        }
    }

    if (bRet) {
        if (sNumberStrTemp[0] == '-') {
            bNegative = true;
            sNumberStrTemp = sNumberStrTemp.substr(::strlen("-"));
        }
    }

    if (bRet) {
        if (sNumberStrTemp.empty()) {
            bRet = false;
        }
    }

    if (bRet) {
        for (i = 0; i < sNumberStrTemp.length(); i++) {
            if (!IsDigitChar(sNumberStrTemp[i])) {
                bRet = false;
                break;
            }
        }
    }

    if (bRet) {
        if (pRetSignedInt != NULL) {
            *pRetSignedInt = StrToL(sNumberStrTemp, 10);
            if (bNegative) {
                *pRetSignedInt = -*pRetSignedInt;
            }
        }
    }

    return bRet;
}

bool GetUnsignedIntFormStr(const std::string &sNumberStr, unsigned int *pRetUnsignedInt /*= NULL*/)
{
    bool bRet = true;
    std::string sNumberStrTemp = sNumberStr;
    bool bHex = false;
    unsigned int i;

    if (pRetUnsignedInt != NULL) {
        *pRetUnsignedInt = 0;
    }

    if (bRet) {
        if (sNumberStrTemp.empty()) {
            bRet = false;
        }
    }

    if (bRet) {
        if (TK_Tools::CompareNoCaseRightLen(sNumberStrTemp, "0x") == 0) {
            bHex = true;
            sNumberStrTemp = sNumberStrTemp.substr(::strlen("0x"));
        }
    }

    if (bRet) {
        if (sNumberStrTemp.empty()) {
            bRet = false;
        }
    }

    if (bRet) {
        for (i = 0; i < sNumberStrTemp.length(); i++) {
            if (!IsHexDigitChar(sNumberStrTemp[i])) {
                bRet = false;
                break;
            }
        }
    }

    if (bRet) {
        if (pRetUnsignedInt != NULL) {
            *pRetUnsignedInt = StrToUL(sNumberStrTemp, (bHex ? 16 : 10));
        }
    }

    return bRet;
}

bool GetUnsignedCharArrayFormStr(const std::string &sNumberStr, std::vector<unsigned char> *pRetUnsignedCharArray /*= NULL*/)
{
    bool bRet = true;
    std::vector<std::string> arrHexChars = SplitString(sNumberStr);
    unsigned int i;

    if (pRetUnsignedCharArray != NULL) {
        pRetUnsignedCharArray->clear();
    }

    if (bRet) {
        for (i = 0; i < arrHexChars.size(); i++) {
            std::string &s = arrHexChars[i];

            if (s.length() == 0) {
                continue;
            }

            if (s.length() != 2) {
                bRet = false;
                break;
            }

            if (!IsHexDigitChar(s[0]) || !IsHexDigitChar(s[1])) {
                bRet = false;
                break;
            }
        }
    }

    if (bRet) {
        for (i = 0; i < arrHexChars.size(); i++) {
            std::string &s = arrHexChars[i];

            if (s.length() == 0) {
                continue;
            }

            if (pRetUnsignedCharArray != NULL) {
                pRetUnsignedCharArray->push_back((unsigned char)StrToUL(s, 16));
            }
        }
    }

    return bRet;
}

#define FILE_READ_BUF_SIZE       2048

bool GetContentFromFile(const std::tstring& sFilePath, std::string &sFileContent)
{
    int ret = -1;
    int nRet;
    FILE *pFile = NULL;
    char *pBuf = new char[FILE_READ_BUF_SIZE];

    sFileContent = "";

    if (pBuf == NULL) {
        ret = -1;
        goto FAILED;
    }

    pFile = fopen(tstr2str(sFilePath).c_str(), "r+b");
    if (pFile == NULL)  {
        ret = -1;
        goto FAILED;
    }

    while (true) {
        nRet = fread(pBuf, 1, FILE_READ_BUF_SIZE, pFile);
        if (nRet > 0) {
            sFileContent += std::string(pBuf, pBuf + nRet);
        } else if (feof(pFile)) {
            break;
        } else {
            ret = -1;
            goto FAILED;
        }
    }

    ret = 0;

FAILED:
    if (pBuf != NULL) {
        delete [] pBuf;
        pBuf = NULL;
    }

    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return (ret == 0);
}

bool SaveContentToFile(const std::tstring& sFilePath, const std::string &sFileContent)
{
    int ret = -1;
    int nRet;
    FILE *pFile = NULL;
    const char *pCurPos = sFileContent.c_str();
    const char *pEndPos = sFileContent.c_str() + sFileContent.length();

    pFile = fopen(tstr2str(sFilePath).c_str(), "w+b");
    if (pFile == NULL)  {
        ret = -1;
        goto FAILED;
    }

    while (true) {
        int nLeftCount = pEndPos - pCurPos;
        if (nLeftCount <= 0) {
            break;
        }

        nRet = fwrite(pCurPos, 1, nLeftCount, pFile);
        if (nRet > 0) {
            pCurPos += nRet;
        } else {
            ret = -1;
            goto FAILED;
        }
    }

    ret = 0;

FAILED:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return (ret == 0);
}

#define BUF_LEN_MAX 1024
bool GetLinesFromFile(const std::string &sFilePath, std::vector<std::wstring> &lines)
{
    bool bRet = true;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    lines.clear();

    if (bRet) {
        fp = fopen(sFilePath.c_str(), "rb");
        if (fp == NULL) {
            bRet = false;
        }
    }

    if (bRet) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            bRet = false;
        }
    }

    if (bRet) {
        while (true) {
            returnedLine = fgetws(lineBuf, BUF_LEN_MAX, fp);
            if (returnedLine == NULL) {
                if (!feof(fp)) {
                    bRet = false;
                }
                break;
            } else {
                trimedLine = TrimAll(returnedLine);
                if (trimedLine.length() != 0) {
                    lines.push_back(trimedLine);
                }
            }
        }
    }

    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    return bRet;
}

#ifdef WIN32
bool Execute(const std::string &sExePath, const std::string &sArgs, bool bWait)
{
    bool bRet = true;
    BOOL bSuc;
    std::tstring tsExePath = TK_Tools::str2tstr(sExePath);
    std::tstring tsArgs = TK_Tools::str2tstr(sArgs);

    SHELLEXECUTEINFO ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = tsExePath.c_str();
    ShExecInfo.lpParameters = tsArgs.c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;
    bSuc = ShellExecuteEx(&ShExecInfo);
    if (bSuc) {
        if (bWait) {
            WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
        }
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}
#endif // #ifdef WIN32

bool IsSpaceChar(char nCh, const std::string &sSpaceChars /*= " \t"*/)
{
    bool bRet = false;
    std::string::size_type nPos;

    nPos = sSpaceChars.find(nCh);
    if (nPos != std::string::npos) {
        bRet = true;
    }

    return bRet;
}

bool ParseCmdLine(std::vector<std::string> &arrRetArgs, const std::string &sCmdLine, bool bEscapeSupported)
{
    bool bRet = true;
    std::string sArgTemp;
    const char *pCh;
    bool bQuoted;
    bool bEscaped;
    bool bOK;
    unsigned int nUnsignedInt;

    arrRetArgs.clear();

    sArgTemp = "";
    pCh = sCmdLine.c_str();
    bQuoted = false;
    bEscaped = false;
    while (*pCh != '\0') {
        if (bEscapeSupported && bEscaped) {
            if (       *pCh == '\"'
                    || *pCh == '\\'
                    || *pCh == '\a'
                    || *pCh == '\b'
                    //|| *pCh == '\c'
                    //|| *pCh == '\e'
                    || *pCh == '\f'
                    || *pCh == '\n'
                    || *pCh == '\r'
                    || *pCh == '\t'
                    || *pCh == '\v'
                ) {
                sArgTemp += *pCh;
                pCh++;
            } else if (*pCh == 'x') {
                pCh++;

                std::string sTemp(pCh, 0, 2);
                if (sTemp.length() != 2) {
                    bRet = false;
                    break;
                }
                sTemp = std::string("0x") + sTemp;
                bOK = GetUnsignedIntFormStr(sTemp, &nUnsignedInt);
                if (!bOK) {
                    bRet = false;
                    break;
                }
                sArgTemp += (char)nUnsignedInt;

                pCh++;
                pCh++;
            } else {
                sArgTemp += '\\';
                sArgTemp += *pCh;
                pCh++;
            }
            bEscaped = false;
        } else {
            if (*pCh == '\"') {
                if (!bQuoted) {
                    if (!sArgTemp.empty()) {
                        bRet = false;
                        break;
                    }
                    bQuoted = true;
                } else {
                    arrRetArgs.push_back(sArgTemp);
                    sArgTemp = "";
                    bQuoted = false;
                }
            } else {
                if (!bQuoted && IsSpaceChar(*pCh)) {
                    if (!sArgTemp.empty()) {
                        arrRetArgs.push_back(sArgTemp);
                        sArgTemp = "";
                    }
                } else {
                    if (bEscapeSupported && *pCh == '\\') {
                        bEscaped = true;
                    } else {
                        sArgTemp += *pCh;
                    }
                }
            }

            pCh++;
        }
    }
    if (!sArgTemp.empty()) {
        arrRetArgs.push_back(sArgTemp);
        sArgTemp = "";
    }

    return bRet;
}

#ifdef WIN32
std::wstring GetWin32ErrMsg(DWORD nErrCode /*= (DWORD)-1*/)
{
    std::wstring sResult = L"(unknown error)";
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    if (nErrCode == -1) {
        nErrCode = GetLastError();
    }

    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        nErrCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)::LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf)+40)*sizeof(TCHAR));
    ::_snwprintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf),
        TEXT("[nLastErrCode=%d]: %s"),
        nErrCode, lpMsgBuf);

    sResult = (LPCTSTR)lpDisplayBuf;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

    return sResult;
}
#endif // #ifdef WIN32

} // namespace TK_Tools {
