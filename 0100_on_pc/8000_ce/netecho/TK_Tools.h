#ifndef TK_TOOLS_H_78328439059049832905484587290349032985834958435934985
#define TK_TOOLS_H_78328439059049832905484587290349032985834958435934985

#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif // #ifdef WIN32
#include <assert.h>
#include <string>
#include <vector>
#include "SMF_afx.h"

#define INT_T							signed int
#define INT8_T							signed char
#define INT16_T							signed short
#define INT32_T							signed long

#define UINT_T							unsigned int
#define UINT8_T							unsigned char
#define UINT16_T						unsigned short
#define UINT32_T						unsigned long

#define BOOL_T 							UINT8_T

#ifndef BOOT
#define BOOT 							int
#endif

#ifndef TRUE
#define TRUE            				1
#endif

#ifndef FALSE
#define FALSE           				0
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

#define TK_UNUSED_VAR(var)              (var)
#define TK_ARR_LEN(arr)					(sizeof(arr) / sizeof(arr[0]))

#define TK_PRINTF						printf
#ifndef UNDER_CE
#define TK_ASSERT(statement)			assert(statement)
#else
#define TK_ASSERT(statement)
#endif // #ifndef UNDER_CE

#ifdef WIN32
#define TK_MSG(format,...)              fprintf(stdout, format, __VA_ARGS__)
#define TK_ERR(format,...)              fprintf(stderr, format, __VA_ARGS__)
#else
#define TK_MSG(format,...)              fprintf(stdout, format, ## __VA_ARGS__)
#define TK_ERR(format,...)              fprintf(stderr, format, ## __VA_ARGS__)
#endif // #ifdef WIN32

#define TK_TEXT_BUF_SIZE				1024

namespace std {
    typedef basic_string<wchar_t> wstring;
} // namespace std {

namespace TK_Tools {

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

bool SendTextToPeer1(const char *pFormat, ...);
bool SendTextToPeer2(const char *pFormat, ...);
void Assert(int expr);
std::string FormatStr(const char *pFormat, ...);
void FormatStr(std::string &strRet, const char *pFormat, ...);
std::wstring FormatWStr(const wchar_t *pFormat, ...);
void FormatWStr(std::wstring &strRet, const wchar_t *pFormat, ...);
std::vector<std::string> SplitString(const std::string &sText, const std::string &sSpaceChars = " \t");
std::vector<std::wstring> SplitWString(const std::wstring &sText, const std::wstring &sSpaceChars = L" \t");
std::string TrimLeft(const std::string &sText, const std::string &sTrimChars = " \t\r\n");
std::string TrimRight(const std::string &sText, const std::string &sTrimChars = " \t\r\n");
std::string TrimAll(const std::string &sText, const std::string &sTrimChars = " \t\r\n");
#ifdef WIN32
std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage = CP_ACP/*ANSI code page*/);
std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage = CP_ACP/*ANSI code page*/);
std::string tstr2str(const std::tstring &tstrInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
std::tstring str2tstr(const std::string &strInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
#endif // #ifdef WIN32
std::string UpperCase(const std::string &sText);
std::string LowerCase(const std::string &sText);
int CompareCase(const std::string &sText1, const std::string &sText2);
int CompareCaseLeftLen(const std::string &sText1, const std::string &sText2);
int CompareCaseRightLen(const std::string &sText1, const std::string &sText2);
int CompareCaseMinLen(const std::string &sText1, const std::string &sText2);
int CompareNoCase(const std::string &sText1, const std::string &sText2);
int CompareNoCaseLeftLen(const std::string &sText1, const std::string &sText2);
int CompareNoCaseRightLen(const std::string &sText1, const std::string &sText2);
int CompareNoCaseMinLen(const std::string &sText1, const std::string &sText2);
unsigned long StrToUL(const std::string &sNumber, int nBase = 0);
long StrToL(const std::string &sNumber, int nBase = 0);
float StrToFloat(const std::string &sNumber);
bool IsAlphabetChar(unsigned char nChar);
bool IsDigitChar(unsigned char nChar);
bool IsHexDigitChar(unsigned char nChar);

bool GetSignedIntFromStr(const std::string &sNumberStr, int *pRetSignedInt = NULL);
bool GetUnsignedIntFormStr(const std::string &sNumberStr, unsigned int *pRetUnsignedInt = NULL);
bool GetUnsignedCharArrayFormStr(const std::string &sNumberStr, std::vector<unsigned char> *pRetUnsignedCharArray = NULL);

bool IsSpaceChar(char nCh, const std::string &sSpaceChars = " \t");
bool ParseCmdLine(std::vector<std::string> &arrRetArgs, const std::string &sCmdLine);

bool GetContentFromFile(const std::tstring& sFilePath, std::string &sFileContent);
bool SaveContentToFile(const std::tstring& sFilePath, const std::string &sFileContent);

bool Execute(const std::string &sExePath, const std::string &sArgs, bool bWait);
std::wstring GetWin32ErrMsg(DWORD nErrCode = (DWORD)-1);
bool LoadFile(const std::string &sFilePath, std::string &sFileContent);
bool SaveToFile(const std::string &sFilePath, std::string &sFileContent);
bool FileExists(const std::string &sFilePath);
bool CreateFile(const std::string &sFilePath);
bool DeleteFile(const std::string &sFilePath);
void Sleep(unsigned int nMiliSecond);
bool RetainObjectFile(const std::string &sFilePath);
bool ReleaseObjectFile(const std::string &sFilePath);
bool ObjectFileExists(const std::string &sFilePath);
#ifndef WIN32
bool ProcessExists(const std::string &sExeName);
bool GetFileINode(const std::string &sFilePath, std::string &sINode);
#ifndef OSX64
bool GetAbsoluteTimeSpec(unsigned int nMillisecondShift, struct timespec &tp, clockid_t nClockId = CLOCK_REALTIME);
#endif // #ifndef OSX64
#endif // #ifndef WIN32

} // namespace TK_Tools {

#endif // TK_TOOLS_H_78328439059049832905484587290349032985834958435934985
