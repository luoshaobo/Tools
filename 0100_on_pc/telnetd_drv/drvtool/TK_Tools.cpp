#include <windows.h>
#include <list>
#include <map>
#include "TK_Tools.h"

//////////////////////////////////////////////////////////////////////////////////////////
// implementation
//
namespace TK_Tools {

bool SendTextToPeer1(const char *pFormat, ...)
{
    return false;
}

bool SendTextToPeer2(const char *pFormat, ...)
{
    return false;
}

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

std::wstring FormatWStr(const wchar_t *pFormat, ...)
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

void FormatWStr(std::wstring &strRet, const wchar_t *pFormat, ...)
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
    bool bRet = true;
    

#ifndef WINCE
    if (bRet) {
        int nRet;
        nRet = ::unlink(sFilePath.c_str());
        if (nRet != 0) {
            bRet = false;
        }
    }
#else
    if (bRet) {
        BOOL bSuc;
        std::wstring wsFilePath = str2wstr(sFilePath);
        bSuc = ::DeleteFile (wsFilePath.c_str());
        if (!bSuc) {
            bRet = false;
        }
    }
#endif // #ifndef WINCE

    return bRet;
}

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

} // namespace TK_Tools {
