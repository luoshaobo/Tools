#ifndef STUB_DLT_H_INC_
#define STUB_DLT_H_INC_

#include <stdio.h>

#define DLT_IMPORT_CONTEXT(...)
#define DLT_DECLARE_CONTEXT(...)
#define DLT_REGISTER_CONTEXT(...)
#define DLT_REGISTER_APP(...)
#define DLT_UNREGISTER_APP(...)
#define DLT_UNREGISTER_CONTEXT(...)

#ifdef __UT_DLT_DISABLE_PRINTF__
#define DLT_LOG_STRING(...)
#define DLT_LOG_STRINGF(...)
#else
#define DLT_LOG_STRINGF(context, logZone, format, ...) \
    fprintf(stderr, format "\n", __VA_ARGS__)
#define DLT_LOG_STRING(context, logZone, str) \
    fprintf(stderr, "%s\n", str)
#endif

#endif
