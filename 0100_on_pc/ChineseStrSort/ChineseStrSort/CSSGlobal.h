#ifndef CSSGLOBAL_H_278394105790235782894125281248923764184923785278492347823844
#define CSSGLOBAL_H_278394105790235782894125281248923764184923785278492347823844

#ifdef WIN32
#include <windows.h>
#endif // #ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <wchar.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <hash_map>
#include "TK_Tools.h"

using namespace TK_Tools;

#define CSS_LOG_PREFIX               "[CSS] "

#define CSS_LOG_LEVEL_DEBUG           0
#define CSS_LOG_LEVEL_INFO            1
#define CSS_LOG_LEVEL_WARNING         2
#define CSS_LOG_LEVEL_ERROR           3
#define CSS_LOG_LEVEL_CRITICAL        4
#define CSS_LOG_LEVEL_FATAL           5

#ifdef WIN32
#define CSS_DEBUG(format,...)         CSS_Log(CSS_LOG_LEVEL_DEBUG, format, __VA_ARGS__)
#define CSS_INFO(format,...)          CSS_Log(CSS_LOG_LEVEL_INFO, format, __VA_ARGS__)
#define CSS_WARNING(format,...)       CSS_Log(CSS_LOG_LEVEL_WARNING, format, __VA_ARGS__)
#define CSS_ERROR(format,...)         CSS_Log(CSS_LOG_LEVEL_ERROR, format, __VA_ARGS__)
#define CSS_CRITICAL(format,...)      CSS_Log(CSS_LOG_LEVEL_CRITICAL, format, __VA_ARGS__)
#define CSS_FATAL(format,...)         CSS_Log(CSS_LOG_LEVEL_FATAL, format, __VA_ARGS__)
#else // #ifdef WIN32
#define CSS_DEBUG(format,...)         CSS_Log(CSS_LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
#define CSS_INFO(format,...)          CSS_Log(CSS_LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define CSS_WARNING(format,...)       CSS_Log(CSS_LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define CSS_ERROR(format,...)         CSS_Log(CSS_LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define CSS_CRITICAL(format,...)      CSS_Log(CSS_LOG_LEVEL_CRITICAL, format, ## __VA_ARGS__)
#define CSS_FATAL(format,...)         CSS_Log(CSS_LOG_LEVEL_FATAL, format, ## __VA_ARGS__)
#endif // #ifdef WIN32

#define CSS_CURFUNC                   __FUNCTION__

void CSS_Log(int nLogLevel, const char *pFormat, ...);


#define CSS_TIME_LOG_INITIALIZE(t)      DWORD t = GetTickCount();
#define CSS_TIME_LOG_RESET(t)           t = GetTickCount();
#define CSS_TIME_LOG_STEP(t,text)       { DWORD deltaT = GetTickCount() - t; CSS_DEBUG("### [time] %s: %lu,%03lums", text, deltaT / 1000, deltaT % 1000); t = GetTickCount(); }
#define CSS_TIME_LOG_DEINITIALIZE(t)

#define CSS_NEWLINE                   "\n"

namespace CSS {

static const int BUF_LEN_MAX = 1024;

enum ErrorCode {
    EC_OK = 0,
    EC_FAILED,
    EC_NOT_IMPLEMENTED,
    EC_NOT_SUPPORTED,

    EC_NO_MEM_TO_NEW,
    EC_LOOKUP_TABLES_NOT_INITED,
    EC_INDEX_OUT_OF_SCOPE,

    EC_FAILED_TO_OPEN_FHTABLE_FILE,
    EC_INVALID_FORMAT_OF_FHTABLE_FILE,
    EC_FAILED_TO_READ_FHTABLE_FILE,
    EC_INVALID_LINE_IN_FHTABLE_FILE,

    EC_FAILED_TO_OPEN_ULTABLE_FILE,
    EC_INVALID_FORMAT_OF_ULTABLE_FILE,
    EC_FAILED_TO_READ_ULTABLE_FILE,
    EC_INVALID_LINE_IN_ULTABLE_FILE,

    EC_FAILED_TO_OPEN_TSTABLE_FILE,
    EC_INVALID_FORMAT_OF_TSTABLE_FILE,
    EC_FAILED_TO_READ_TSTABLE_FILE,
    EC_INVALID_LINE_IN_TSTABLE_FILE,

    EC_FAILED_TO_OPEN_SURNAMEPINYINTABLE_FILE,
    EC_INVALID_FORMAT_OF_SURNAMEPINYINTABLE_FILE,
    EC_FAILED_TO_READ_SURNAMEPINYINTABLE_FILE,
    EC_INVALID_LINE_IN_SURNAMEPINYINTABLE_FILE,

    EC_FAILED_TO_OPEN_UNICODEPINYINTABLE_FILE,
    EC_INVALID_FORMAT_OF_UNICODEPINYINTABLE_FILE,
    EC_FAILED_TO_READ_UNICODEPINYINTABLE_FILE,
    EC_INVALID_LINE_IN_UNICODEPINYINTABLE_FILE,

    EC_FAILED_TO_OPEN_PHRASEPINYINTABLE_FILE,
    EC_INVALID_FORMAT_OF_PHRASEPINYINTABLE_FILE,
    EC_FAILED_TO_READ_PHRASEPINYINTABLE_FILE,
    EC_INVALID_LINE_IN_PHRASEPINYINTABLE_FILE,


};

class PluginLoader
{
public:
    PluginLoader(const std::string &dllDir, const std::string &dllName);
    ~PluginLoader();

    ErrorCode LoadPlugin();
    ErrorCode UnLoadPlugin();
    ErrorCode GetSymbolAddr(const std::string &symbol, void **symbalAddr);

private:
    ErrorCode DoLoadPlugin();

private:
    std::string m_dllDir;
    std::string m_dllName;
    HMODULE m_hModule;
};

} // namespace CSS {

#endif // #ifndef CSSGLOBAL_H_278394105790235782894125281248923764184923785278492347823844
