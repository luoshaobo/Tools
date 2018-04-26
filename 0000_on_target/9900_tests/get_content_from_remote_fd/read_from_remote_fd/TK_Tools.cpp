#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <unistd.h>
#include "TK_Tools.h"

#define MAX_STR_LEN 1024

//////////////////////////////////////////////////////////////////////////////////////////
// implementation
//
namespace TK_Tools {

void Assert(int expr)
{
    if (!expr)
    {
        volatile int i = 0;
        while (true)
        {
            i++;
        }
    }
}

std::string FormatStr(const char *pFormat, ...)
{
    std::string strRet;
    va_list args;
    int len;
    char *buffer;

    va_start(args, pFormat);
#ifdef WIN32
    len = _vscprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
#else
    len = MAX_STR_LEN + 1;
#endif // #ifdef WIN32
    buffer = (char*)malloc(len * sizeof(char));
    if (buffer != NULL) {
        vsprintf(buffer, pFormat, args);
        strRet = buffer;
        free(buffer);
    } else {
        strRet = "";
    }
    va_end(args);

    return strRet;
}

void FormatStr(std::string &strRet, const char *pFormat, ...)
{
    va_list args;
    int len;
    char *buffer;

    va_start(args, pFormat);
#ifdef WIN32
    len = _vscprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
#else
    len = MAX_STR_LEN + 1;
#endif // #ifdef WIN32
    buffer = (char*)malloc(len * sizeof(char));
    if (buffer != NULL) {
        vsprintf(buffer, pFormat, args);
        strRet = buffer;
        free(buffer);
    } else {
        strRet = "";
    }
    va_end(args);
}

std::wstring FormatWStr(const wchar_t *pFormat, ...)
{
    std::wstring strRet;
//    va_list args;
//    int len;
//    wchar_t *buffer;

//    va_start(args, pFormat);
//#ifdef WIN32
//    len = _vscwprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
//#else
//    len = MAX_STR_LEN + 1;
//#endif // #ifdef WIN32
//    buffer = (wchar_t*)malloc(len * sizeof(wchar_t));
//    if (buffer != NULL) {
//#ifdef WIN32
//        wvsprintf(buffer, pFormat, args);
//#else
//        vswprintf(buffer, len, pFormat, args);
//#endif // #ifdef WIN32
//        strRet = buffer;
//        free(buffer);
//    } else {
//        strRet = L"";
//    }
//    va_end(args);

    return strRet;
}

void FormatWStr(std::wstring &strRet, const wchar_t *pFormat, ...)
{
//    va_list args;
//    int len;
//    wchar_t *buffer;

//    va_start(args, pFormat);
//#ifdef WIN32
//    len = _vscwprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
//#else
//    len = MAX_STR_LEN + 1;
//#endif // #ifdef WIN32
//    buffer = (wchar_t*)malloc(len * sizeof(wchar_t));
//    if (buffer != NULL) {
//#ifdef WIN32
//        wvsprintf(buffer, pFormat, args);
//#else
//        vswprintf(buffer, len, pFormat, args);
//#endif // #ifdef WIN32
//        strRet = buffer;
//        free(buffer);
//    } else {
//        strRet = L"";
//    }
//    va_end(args);
}

std::vector<std::string> SplitString(const std::string &sText, const std::string &sSpaceChars /*= " \t"*/)
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

std::vector<std::wstring> SplitWString(const std::wstring &sText, const std::wstring &sSpaceChars /*= L" \t"*/)
{
    std::vector<std::wstring> vResult;

    const wchar_t *p = sText.c_str();
    std::wstring sTemp;
    for (; *p != L'\0'; p++) {
        std::wstring::size_type nPos = sSpaceChars.find(*p);
        if (nPos == std::wstring::npos) {               // non space char
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

std::string TrimLeft(const std::string &sText, const std::string &sTrimChars /*= " \t\r\n"*/)
{
    const char *p = sText.c_str();
    for (; *p != '\0'; p++) {
        std::string::size_type nPos = sTrimChars.find(*p);
        if (nPos == std::string::npos) {
            break;
        }
    }

    return std::string(p);
}

std::string TrimRight(const std::string &sText, const std::string &sTrimChars /*= " \t\r\n"*/)
{
    if (sText.length() == 0) {
        return std::string("");
    }

    const char *pFirst = sText.c_str();
    const char *p = pFirst + sText.length() - 1;
    for (; p >= pFirst; p--) {
        std::string::size_type nPos = sTrimChars.find(*p);
        if (nPos == std::string::npos) {
            break;
        }
    }
    p++;

    std::string sResult = sText.substr(0, p - pFirst);
    return sResult;
}

std::string TrimAll(const std::string &sText, const std::string &sTrimChars /*= " \t\r\n"*/)
{
    std::string sResult;
    
    sResult = TrimLeft(sText, sTrimChars);
    sResult = TrimRight(sResult, sTrimChars);

    return sResult;
}

std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage)
{
    std::string strOutput;
//    AutoFreeArrayPtr<char> afapBuf;
//    char *pBuf;
//    unsigned nBufSize;
//    int nRet;
//    DWORD nLastError = 0;

//    nRet = ::WideCharToMultiByte(
//        nCodePage,
//        0,
//        wstrInput.c_str(),
//        wstrInput.length(),
//        NULL,
//        0,
//        "?",
//        NULL
//    );
//    if (nRet <= 0)
//    {
//        nLastError = ::GetLastError();
//        goto FAILED;
//    }

//    nBufSize = nRet + 1;
//    pBuf = new char[nBufSize];
//    if (pBuf == NULL) {
//        TK_ASSERT(0);
//        goto FAILED;
//    }
//    afapBuf.Set(pBuf);

//    nRet = ::WideCharToMultiByte(
//        nCodePage,
//        0,
//        wstrInput.c_str(),
//        wstrInput.length(),
//        pBuf,
//        nBufSize,
//        "?",
//        NULL
//    );
//    if (nRet <= 0)
//    {
//        nLastError = ::GetLastError();
//        goto FAILED;
//    }
//    pBuf[nRet] = '\0';

//    strOutput = pBuf;
    
//FAILED:
    return strOutput;
}

std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage)
{
    std::wstring wstrOutput;
//    AutoFreeArrayPtr<wchar_t> afapBuf;
//    wchar_t *pBuf;
//    unsigned nBufSize;
//    int nRet;
//    DWORD nLastError = 0;

//    nRet = ::MultiByteToWideChar(
//        nCodePage,
//        0,
//        strInput.c_str(),
//        strInput.length(),
//        NULL,
//        0
//    );
//    if (nRet <= 0)
//    {
//        nLastError = ::GetLastError();
//        goto FAILED;
//    }

//    nBufSize = nRet + 1;
//    pBuf = new wchar_t[nBufSize];
//    if (pBuf == NULL) {
//        TK_ASSERT(0);
//        goto FAILED;
//    }
//    afapBuf.Set(pBuf);

//    nRet = ::MultiByteToWideChar(
//        nCodePage,
//        0,
//        strInput.c_str(),
//        strInput.length(),
//        pBuf,
//        nBufSize
//    );
//    if (nRet <= 0)
//    {
//        nLastError = ::GetLastError();
//        goto FAILED;
//    }
//    pBuf[nRet] = L'\0';

//    wstrOutput = pBuf;
    
//FAILED:
    return wstrOutput;
}

std::string tstr2str(const std::tstring &tstrInput, unsigned long nCodePage)
{
//#if defined(UNICODE) || defined(_UNICODE)
//        return wstr2str(tstrInput, nCodePage);
//#else
        return tstrInput;
//#endif
}

std::tstring str2tstr(const std::string &strInput, unsigned long nCodePage)
{
//#if defined(UNICODE) || defined(_UNICODE)
//        return str2wstr(strInput, nCodePage);
//#else
        return strInput;
//#endif
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

long StrToL(const std::string &sNumber, int nBase /*= 10*/)
{
    long nNumber = 0;

    nNumber = strtol(sNumber.c_str(), NULL, nBase);
    return nNumber;
}

float StrToFloat(const std::string &sNumber)
{
    float fResult = 0.0f;

    sscanf(sNumber.c_str(), "%f", &fResult);
    return fResult;
}

bool IsAlphabetChar(unsigned char nChar)
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

bool IsDigitChar(unsigned char nChar)
{
    bool bRet = false;
    
    if ('0' <= nChar && nChar <= '9') {
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

bool IsHexDigitChar(unsigned char nChar)
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

bool FileExists(const std::string &sFilePath)
{
    bool bRet = false;
    FILE *pFile;
    
    if (sFilePath == "-") {
        bRet = true;
    } else {
        pFile = fopen(sFilePath.c_str(), "rb");
        if (pFile != NULL) {
            fclose(pFile);
            bRet = true;
        } else {
            bRet = false;
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

    if (sFilePath == "-") {
        pFile = stdin;
    } else {
        pFile = fopen(tstr2str(sFilePath).c_str(), "r+b");
    }
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
        if (pFile != stdin) {
            fclose(pFile);
        }
        pFile = NULL;
    }

    return (ret == 0);
}

bool GetContentFromFile(const std::tstring& sFilePath, std::vector<unsigned char> &vFileContent)
{
    int ret = -1;
    int nRet;
    FILE *pFile = NULL;
    char *pBuf = new char[FILE_READ_BUF_SIZE];

    vFileContent.clear();
    vFileContent.reserve(1024 * 1024);

    if (pBuf == NULL) {
        ret = -1;
        goto FAILED;
    }

    if (sFilePath == "-") {
        pFile = stdin;
    } else {
        pFile = fopen(tstr2str(sFilePath).c_str(), "r+b");
    }
    if (pFile == NULL)  {
        ret = -1;
        goto FAILED;
    }

    while (true) {
        nRet = fread(pBuf, 1, FILE_READ_BUF_SIZE, pFile);
        if (nRet > 0) {
            vFileContent.insert(vFileContent.end(), pBuf, pBuf + nRet);
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
        if (pFile != stdin) {
            fclose(pFile);
        }
        pFile = NULL;
    }

    return (ret == 0);
}

bool GetContentFromFile(int fd, std::vector<unsigned char> &vFileContent)
{
    int ret = -1;
    int nRet;
    char *pBuf = new char[FILE_READ_BUF_SIZE];

    vFileContent.clear();
    vFileContent.reserve(1024 * 1024);

    if (pBuf == NULL) {
        ret = -1;
        goto FAILED;
    }

    lseek(fd, SEEK_SET, 0);

    while (true) {
        nRet = read(fd, pBuf, FILE_READ_BUF_SIZE);
        if (nRet > 0) {
            vFileContent.insert(vFileContent.end(), pBuf, pBuf + nRet);
        } else if (nRet == 0) {
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

    return (ret == 0);
}

bool SaveContentToFile(const std::tstring& sFilePath, const std::string &sFileContent)
{
    int ret = -1;
    int nRet;
    FILE *pFile = NULL;
    const char *pCurPos = sFileContent.c_str();
    const char *pEndPos = sFileContent.c_str() + sFileContent.length();

    if (sFilePath == "-") {
        pFile = stdout;
    } else {
        pFile = fopen(tstr2str(sFilePath).c_str(), "w+b");
    }
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
        if (pFile != stdout) {
            fclose(pFile);
        }
        pFile = NULL;
    }

    return (ret == 0);
}

bool SaveContentToFile(const std::tstring& sFilePath, const std::vector<unsigned char> &vFileContent, unsigned int nSkippedFrameCount /*= 0*/)
{
    int ret = -1;
    int nRet;
    FILE *pFile = NULL;
    const unsigned char *pCurPos = &vFileContent[0];
    const unsigned char *pEndPos = pCurPos + vFileContent.size();

    if (sFilePath == "-") {
        pFile = stdout;
    } else {
        pFile = fopen(tstr2str(sFilePath).c_str(), "w+b");
    }
    if (pFile == NULL) {
        ret = -1;
        goto FAILED;
    }

    if (nSkippedFrameCount != 0) {
        ret = fseek(pFile, vFileContent.size() * nSkippedFrameCount, SEEK_SET);
        if (ret != 0) {
            ret = 0;
            fprintf(stderr, "*** ERROR: can't seek on this file: %s\n", sFilePath.c_str());
        }
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
        if (pFile != stdout) {
            fclose(pFile);
        }
        pFile = NULL;
    }

    return (ret == 0);
}

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

} // namespace TK_Tools {
