#pragma once
#include <windows.h>
#include <assert.h>
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

#ifndef BOOT
#define BOOT 							int
#endif

#ifndef TRUE
#define TRUE            				1
#endif

#ifndef FALSE
#define FALSE           				0
#endif

#define TK_UNUSED_VAR(var)              (var)
#define TK_ARR_LEN(arr)					(sizeof(arr) / sizeof(arr[0]))

#define TK_PRINTF						printf
#define TK_ASSERT(statement)			assert(statement)

#define TK_MSG(format,...)              fprintf(stdout, format, __VA_ARGS__)
#define TK_ERR(format,...)              fprintf(stderr, format, __VA_ARGS__)

#define TK_TEXT_BUF_SIZE				1024

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

std::string FormatStr(const char *pFormat, ...);
void FormatStr(std::string &strRet, const char *pFormat, ...);
std::string TrimLeft(const std::string &sText, const std::string &sTrimChars = " \t\r\n");
std::string TrimRight(const std::string &sText, const std::string &sTrimChars = " \t\r\n");
std::string TrimAll(const std::string &sText, const std::string &sTrimChars = " \t\r\n");
std::vector<std::string> SplitString(const std::string &sText, const std::string &sSpaceChars = " \t");
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
bool LoadFromFile(const std::string &sFilePath, std::string &sFileContent);
bool SaveToFile(const std::string &sFilePath, std::string &sFileContent);
bool FileExists(const std::string &sFilePath);

} // namespace TK_Tools {
