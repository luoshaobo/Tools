#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <QObject>
#include "HmiUtil.h"

static void HMI_CL_OutputToConsole(const char *pText, unsigned int nLen)
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

static int HMI_Log_nLevel = HMI_LOG_LEVEL_DEBUG;
void HMI_Log(const char *prefix, int nLogLevel, const char *pFormat, ...)
{
    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

    if (nLogLevel < HMI_Log_nLevel) {
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
            sOutput += prefix;
            sOutput += "################################### ";
            sOutput += buffer;
            if (sOutput.length() >= 1 && sOutput[sOutput.length() - 1] != '\n') {
                sOutput += "\n";
            }
            //qWarning(sOutput.c_str());
            //HMI_CL_OutputToConsole(sOutput.c_str(), sOutput.length());
            //HMI_CL_OutputToConsole("\n", 1);
            printf("%s", sOutput.c_str());
            
        }
        va_end(args);
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return;
}
