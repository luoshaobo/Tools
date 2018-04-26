#include "StdAfx.h"
#include "Tools.h"

namespace TK_Tools {

std::string FormatStr(const char *pFormat, ...)
{
    std::string strRet;
    va_list args;
    int len;
    char *buffer;

    va_start(args, pFormat);
    len = _vscprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
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
    len = _vscprintf(pFormat, args) + 1; // NOTE: _vscprintf doesn't count terminating '\0'
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

long StrToL(const std::string &sNumber, int nBase /*= 0*/)
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

bool LoadFromFile(const std::string &sFilePath, std::string &sFileContent)
{
    bool bRet = false;
    FILE *pFile = NULL;
    long nFileSize = 0;
    char *pFileContentBuf = NULL;
    AutoFreeArrayPtr<char> afap;
    size_t nRead;

    pFile = fopen(sFilePath.c_str(), "rb");
    if (pFile == NULL) {
        DWORD nErr = GetLastError();
        bRet = false;
        goto FAILED;
    }
    fseek(pFile, 0, SEEK_END);
    nFileSize = ftell(pFile);
    pFileContentBuf = new char[nFileSize + 1];
    TK_ASSERT(pFileContentBuf != NULL);
    pFileContentBuf[nFileSize] = '\0';
    afap.Set(pFileContentBuf);

    fseek(pFile, 0, SEEK_SET);
    nRead = fread(pFileContentBuf, 1, nFileSize, pFile);
    if (nRead != nFileSize) {
        bRet = false;
        goto FAILED;
    }

    sFileContent = pFileContentBuf;
    bRet = true;

FAILED:
    if (pFile != NULL) {
        fclose(pFile);
        pFile = NULL;
    }

    return bRet;
}

bool SaveToFile(const std::string &sFilePath, std::string &sFileContent)
{
    bool bRet = false;
    FILE *pFile = NULL;
    size_t nWrite;

    pFile = fopen(sFilePath.c_str(), "w+b");
    if (pFile == NULL) {
        DWORD nErr = GetLastError();
        bRet = false;
        goto FAILED;
    }
    
    nWrite = fwrite(sFileContent.c_str(), 1, sFileContent.size(), pFile);
    if (nWrite != sFileContent.size()) {
        bRet = false;
        goto FAILED;
    }

    bRet = true;

FAILED:
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

} // namespace TK_Tools {
