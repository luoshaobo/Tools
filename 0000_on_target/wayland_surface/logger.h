#ifndef LOGGER_H_7623894100423418924728942178423478378
#define LOGGER_H_7623894100423418924728942178423478378

#include "log.h"

#if 1
    #define LOG_INFO(...) {}
    #define LOG_WARN(...) {}
    #define LOG_ERROR(...) {}
    #define LOG_DEBUG(...) {}

    #define INIT_LOGGER(...) {}
    #define UNINIT_LOGGER() {}

    #define REGISTER_BACKEND(...) {}
    #define UNREGISTER_BACKEND(...) {}
#else
    #define LOG_INFO            LOG_GEN_PRINTF_NL
    #define LOG_WARN            LOG_GEN_PRINTF_NL
    #define LOG_ERROR           LOG_GEN_PRINTF_NL
    #define LOG_DEBUG           LOG_GEN_PRINTF_NL

    #define INIT_LOGGER(...) {}
    #define UNINIT_LOGGER() {}

    #define REGISTER_BACKEND(...) {}
    #define UNREGISTER_BACKEND(...) {}
#endif // if 0

#endif /* #ifndef LOGGER_H_7623894100423418924728942178423478378 */
