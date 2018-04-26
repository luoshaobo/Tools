#include "StdAfx.h"
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
#include "Log.h"

using namespace TK_Tools;

#ifndef WIN32
static void MS_OutputToConsole(const char *pText, unsigned int nLen)
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
#else
static void MS_OutputToFile(const char *pText, unsigned int nLen)
{
    static FILE *pLogFile = NULL;

    if (pLogFile == NULL) {
        pLogFile = fopen("d:\\mp.log", "wb");
    }

    if (pLogFile != NULL) {
        if (pText != NULL && nLen != 0) {
            fwrite(pText, 1, nLen, pLogFile);
            fflush(pLogFile);
        }
    }
}
#endif // #ifndef WIN32

static int MS_Log_nLevel = MS_LOG_LEVEL_DEBUG;
void MS_Log(int nLogLevel, const char *pFormat, ...)
{
    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

    if (nLogLevel < MS_Log_nLevel) {
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
            sOutput += MS_LOG_PREFIX;
            sOutput += FormatStr("[%09lu] ", ::GetTickCount());
            sOutput += buffer;

#ifndef WIN32
            qWarning(sOutput.c_str());
            MS_OutputToConsole(sOutput.c_str(), sOutput.length());
            MS_OutputToConsole("\n", 1);
#else
            printf(sOutput.c_str());
            printf("\n");

            MS_OutputToFile(sOutput.c_str(), sOutput.length());
            MS_OutputToFile("\n", 1);
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
