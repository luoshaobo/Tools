#ifndef __STUB_DLT_H_INC__
#define __STUB_DLT_H_INC__

#include <stdio.h>

#define DLT_IMPORT_CONTEXT(...)

#ifdef __UT_DLT_DISABLE_PRINTF__
#define DLT_LOG_STRING(...)
#define DLT_LOG_STRINGF(...)
#else
#define DLT_LOG_STRINGF(context, logZone, format, ...) \
    printf(format "\n", __VA_ARGS__)
#define DLT_LOG_STRING(context, logZone, str) \
    printf("%s\n", str)
#endif

#endif
