#include "CSSGlobal.h"

#ifndef WIN32
static void CSS_OutputToConsole(const char *pText, unsigned int nLen)
{
    static FILE *pConsole = NULL;

    if (pConsole == NULL) {
        pConsole = fopen("/dev/console", "wb");
    }

    if (pConsole != NULL) {
        if (pText != NULL && nLen != 0) {
            fwrite(pText, 1, nLen, pConsole);
        }
    }
}
#endif // #ifndef WIN32

static int CSS_Log_nLevel = CSS_LOG_LEVEL_DEBUG;
void CSS_Log(int nLogLevel, const char *pFormat, ...)
{
    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

    if (nLogLevel < CSS_Log_nLevel) {
        nError = 2;
    }

    if (nError == 0) {
        if (pFormat == NULL) {
            nError = 1;
        }
    }

    if (nError == 0) {
        va_start(args, pFormat);
        len = vsnprintf(NULL, 0, pFormat, args);
        if (len <= 0) {
            len = 1024;
        }
        buffer = (char *)malloc((len + 1) * sizeof(char));
        if (buffer == NULL) {
           nError = -1;
        }
        va_end(args);
    }

    if (nError == 0) {
        va_start(args, pFormat);
        vsnprintf(buffer, len + 1, pFormat, args);
        buffer[len] = '\0';
        {
            std::string sOutput;
            sOutput += CSS_LOG_PREFIX;
            sOutput += buffer;

#ifndef WIN32
            qWarning(sOutput.c_str());
            CSS_OutputToConsole(sOutput.c_str(), sOutput.length());
            CSS_OutputToConsole("\n", 1);
#else
            printf(sOutput.c_str());
            printf("\n");
#endif // #ifndef WIN32
        }
        va_end(args);
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return;
}

namespace CSS {

PluginLoader::PluginLoader(const std::string &dllDir, const std::string &dllName)
    : m_dllDir(dllDir)
    , m_dllName(dllName)
    , m_hModule(NULL)
{

}

PluginLoader::~PluginLoader()
{
    UnLoadPlugin();
}

ErrorCode PluginLoader::LoadPlugin()
{
    ErrorCode ec = EC_OK;

    if (ec == EC_OK) {
        if (m_hModule == NULL) {
            ec = DoLoadPlugin();
        }
    }

    return ec;
}

ErrorCode PluginLoader::DoLoadPlugin()
{
    ErrorCode ec = EC_OK;
    std::string dllPathName;
    unsigned int i;

    if (ec == EC_OK) {
        FormatStr(dllPathName, "%s\\%s.dll", m_dllDir.c_str(), m_dllName.c_str());
        for (i = 0; i < dllPathName.length(); ++i) {
            if (dllPathName[i] == '/') {
                dllPathName[i] = '\\';
            }
        }
    }

    if (ec == EC_OK) {
        m_hModule = ::LoadLibrary(str2tstr(dllPathName).c_str());
        if (m_hModule == NULL) {
            ec = EC_FAILED;
        }
    }

    return ec;
}

ErrorCode PluginLoader::UnLoadPlugin()
{
    ErrorCode ec = EC_OK;

    if (ec == EC_OK) {
        if (m_hModule != NULL) {
            ::FreeLibrary(m_hModule);
            m_hModule = NULL;
        }
    }

    return ec;
}

ErrorCode PluginLoader::GetSymbolAddr(const std::string &symbol, void **symbalAddr)
{
    ErrorCode ec = EC_OK;
    FARPROC pSymbol = NULL;

    if (ec == EC_OK) {
        if (m_hModule == NULL) {
            ec = EC_FAILED;
        }
    }

    if (ec == EC_OK) {
        pSymbol = ::GetProcAddress(m_hModule, symbol.c_str());
        if (pSymbol == NULL) {
            ec = EC_FAILED;
        }
    }

    if (ec == EC_OK) {
        *symbalAddr = (void *)pSymbol;
    }

    return ec;
}

} // namespace CSS {
