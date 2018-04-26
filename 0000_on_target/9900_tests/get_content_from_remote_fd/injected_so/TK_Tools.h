#ifndef TK_TOOLS_H__7823848237237472342347234872384238478
#define TK_TOOLS_H__7823848237237472342347234872384238478

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#endif // #ifdef WIN32

#include <string>
#include <vector>

#define INT_T							signed int
#define INT8_T							signed char
#define INT16_T							signed short
#define INT32_T							signed long

#define UINT_T							unsigned int
#define UINT8_T							unsigned char
#define UINT16_T						unsigned short
#define UINT32_T						unsigned long

#define BOOL_T 							UINT8_T

#ifndef BOOL
#define BOOL 							int
#endif

#ifndef TRUE
#define TRUE            				1
#endif

#ifndef FALSE
#define FALSE           				0
#endif

#define TK_MAX_UINT             ((unsigned int)-1)

#ifndef WIN32
#define DWORD long
#define CP_ACP 0
#define _vsnprintf vsnprintf
#endif // #ifndef WIN32

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

#define TK_UNUSED_VAR(var)              (&var)
#define TK_ARR_LEN(arr)					(sizeof(arr) / sizeof(arr[0]))

#define TK_PRINTF						SendTextToPeer1
#define TK_ASSERT(statement)			\
	if (!(statement)) \
	{ \
		TK_PRINTF("*** Assert in %s()[%s: %lu]: %s", __FUNCTION__, __FILE__, __LINE__, #statement); \
		TK_Tools::Assert(statement); \
	}

#define TK_TEXT_BUF_SIZE				1024

typedef void (*MyGenericGUI_FuncType)(unsigned short id, signed short value, unsigned int data);

namespace TK_Tools {

enum DebugServerInstanceId{
	DSIID_DEF = 0,
	DSIID_NAVI = DSIID_DEF,
	DSIID_TLM,
	DSIID_AV,
	DSIID_SYSCONF,
	DSIID_SNS,
	DSIID_COUNT
};

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
std::string wstr2str(const std::wstring &wstrInput, unsigned long nCodePage = CP_ACP/*ANSI code page*/);
std::wstring str2wstr(const std::string &strInput, unsigned long nCodePage = CP_ACP/*ANSI code page*/);
std::string tstr2str(const std::tstring &tstrInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
std::tstring str2tstr(const std::string &strInput, unsigned long nCodePage  = CP_ACP/*ANSI code page*/);
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

bool FileExists(const std::string &sFilePath);

bool GetContentFromFile(const std::tstring& sFilePath, std::string &sFileContent);
bool SaveContentToFile(const std::tstring& sFilePath, const std::string &sFileContent);
bool SaveContentToFile(const std::tstring& sFilePath, const std::vector<unsigned char> &aFileContent);

bool Execute(const std::string &sExePath, const std::string &sArgs, bool bWait);
#ifdef WIN32
std::wstring GetWin32ErrMsg(DWORD nErrCode = (DWORD)-1);
#endif // #ifdef WIN32

} //namespace TK_Tools {

#endif // #ifndef TK_TOOLS_H__7823848237237472342347234872384238478
