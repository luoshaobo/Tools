#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libgen.h>
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

static bool log_enabled = true;

void log_enable(bool enabled)
{
    log_enabled = enabled;
}

void log_printf(const char *pFormat, ...)
{
    if (!log_enabled) {
        return;
    }

    int nError = 0;
    va_list args;
    int len = 0;
    char *buffer = NULL;

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
        printf(buffer);                                                       // to be changed to any function which can output a string
        va_end(args);
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return;
}

const char *log_basename(const char *path)
{
    const char *pRet = path;

    if (path != NULL) {
        char *p = strrchr((char *)path, '/');
        if (p != NULL) {
            pRet = (const char *)(p + 1);
        }
    }

    return pRet;
}

const char *log_strcat(unsigned int arg_count, ...)
{
    char *p = NULL;
    va_list arg_ptr;
    unsigned int len = 0;
    unsigned int i;
    char *pArg;

    va_start(arg_ptr, arg_count);
    for (i = 0; i < arg_count; i++) {
        pArg = va_arg(arg_ptr, char *);
        if (pArg != NULL) {
            len += strlen(pArg);
        }
    }
    va_end(arg_ptr);

    p = (char *)malloc(len + 1);
    if (p != NULL) {
        p[0] = '\0';
        va_start(arg_ptr, arg_count);
        for (i = 0; i < arg_count; i++) {
            pArg = va_arg(arg_ptr, char *);
            if (pArg != NULL) {
                strcat(p, pArg);
            }
        }
        va_end(arg_ptr);
    }

    return p;
}

#ifdef __cplusplus
} /* extern "C" { */
#endif
