#define LOG_FULL_FUNC_SIGNATURE __PRETTY_FUNCTION__
#define LOG_BASE_FILE_NAME log_basename(__FILE__)

#define LOG_PRINTF(format,...) do { log_printf(format, ## __VA_ARGS__); } while(0)
#define LOG_GEN() do { log_printf("=== LOG_GEN: [%s: %u: %s]\n", LOG_BASE_FILE_NAME, __LINE__, LOG_FULL_FUNC_SIGNATURE); } while(0)
#define LOG_GEN_PRINTF(format,...) do { const char *s = NULL; log_printf(s=log_strcat(2, "=== LOG_GEN: [%s: %u: %s] ", format), LOG_BASE_FILE_NAME, __LINE__, LOG_FULL_FUNC_SIGNATURE, ## __VA_ARGS__); if (s != NULL) { free((void *)s); } } while(0)
#define LOG_GEN_PRINTF_NL(format,...) do { const char *s = NULL; log_printf(s=log_strcat(3, "=== LOG_GEN: [%s: %u: %s] ", format, "\n"), LOG_BASE_FILE_NAME, __LINE__, LOG_FULL_FUNC_SIGNATURE, ## __VA_ARGS__); if (s != NULL) { free((void *)s); } } while(0)

#ifdef __cplusplus
extern "C" {
#endif

extern void log_printf(const char *format, ...);
extern const char *log_basename(const char *path);
extern const char *log_strcat(unsigned int arg_count, ...);

#ifdef __cplusplus
} /* extern "C" { */
#endif

extern int main1(int argc, char* argv[]);
extern int main2(int argc, char* argv[]);

int main(int argc, char *argv[])
{
    return main2(argc, argv);
}

// log.cpp
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libgen.h>

#ifdef __cplusplus
extern "C" {
#endif

void log_printf(const char *pFormat, ...)
{
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
