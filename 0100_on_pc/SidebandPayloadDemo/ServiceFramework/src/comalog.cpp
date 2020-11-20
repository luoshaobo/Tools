#include "comalog.h"
#include <string>
#include <iostream>
#include <stdarg.h>

//DLT_CONTEXT is normally set at compile time (CMakeLists.txt)
#ifndef COMALOG_DLT_CONTEXT
#define COMALOG_DLT_CONTEXT none
#endif

#ifndef COMALOG_NO_DLT
DLT_DECLARE_CONTEXT (COMALOG_DLT_CONTEXT);
#endif

//char comalog_app_name[5] = "NONE";
std::string comalog_app_name = std::string("NONE");
void COMALOG_INIT(const char *app_name, const char *description)
{
    UNUSED(app_name);
    UNUSED(description);

#ifdef LOG_USE_COLOURFUL_CONSOLE
    std::cout << std::string(app_name) << " [COLOURMODE ON]\n" ;
#else
    std::cout << std::string(app_name) << " [COLOURMODE OFF]\n";
#endif //LOG_USE_COLOURFUL_CONSOLE

#ifndef COMALOG_NO_DLT
    DLT_REGISTER_APP(app_name, description);
#ifdef COMALOG_DEBUG_PRINTS
    DLT_REGISTER_CONTEXT_LL_TS(COMALOG_DLT_CONTEXT, app_name, description, DLT_LOG_VERBOSE, DLT_TRACE_STATUS_ON);
#else
    DLT_REGISTER_CONTEXT(COMALOG_DLT_CONTEXT, app_name, description);
#endif //COMALOG_DEBUG_PRINTS
#endif //COMALOG_NO_DLT
    // store app-wide max-four-letter cstring code (trace prefixes, DLT log identifier...)
    //strncpy_s(comalog_app_name, sizeof(comalog_app_name), app_name, sizeof(comalog_app_name) - 1);
    //comalog_app_name[4] = '\0';
    comalog_app_name = std::string(app_name).substr(0, 4);
}
void COMALOG_DEINIT()
{
#ifndef COMALOG_NO_DLT
    DLT_UNREGISTER_CONTEXT(COMALOG_DLT_CONTEXT);
    DLT_UNREGISTER_APP();
#endif //COMALOG_NO_DLT
}
#define COMALOG_BUF_LEN 10000
void COMALOG(ComaLogLevel level, const char *format, ...)
{
    return;///////////

#ifndef COMALOG_DEBUG_PRINTS
    if (level <= DLT_LOG_INFO)
    {
#endif //COMALOG_DEBUG_PRINTS
        char str[COMALOG_BUF_LEN];

        va_list argptr;
        va_start(argptr, format);
        vsnprintf(str, COMALOG_BUF_LEN - 1, format, argptr);
        va_end(argptr);

        //snprintf(str, COMALOG_BUF_LEN-1, format, args);
        /*
         char* str;
         va_list args;
         va_start(args, format);
         vasprintf(&str, format, args);*/
#ifndef COMALOG_NO_DLT
        DLT_LOG_STRING(COMALOG_DLT_CONTEXT, (DltLogLevelType) level, str);
#endif //COMALOG_NO_DLT
#ifdef COMALOG_DEBUG_PRINTS
    #ifdef LOG_USE_COLOURFUL_CONSOLE
        switch (level) {
        case COMALOG_VERBOSE: {
          std::cout << comalog_app_name << " [" << ANSI_COLOR_BLUE << std::string(log_level_str[level - 1]) << ANSI_COLOR_RESET <<"]: " << std::string(str);
          break;
        }
        case COMALOG_DEBUG: {
          std::cout << comalog_app_name << " [" << ANSI_COLOR_BLUE << std::string(log_level_str[level - 1]) << ANSI_COLOR_RESET <<"]: " << std::string(str);
          break;
        }
        case COMALOG_INFO: {
          std::cout << comalog_app_name << " [" << ANSI_COLOR_GREEN << std::string(log_level_str[level - 1]) << ANSI_COLOR_RESET <<"]: " << std::string(str);
          break;
        }
        case COMALOG_WARN: {
          std::cout << comalog_app_name << " [" << ANSI_COLOR_YELLOW << std::string(log_level_str[level - 1]) << ANSI_COLOR_RESET <<"]: " << std::string(str);
          break;
        }
        case COMALOG_ERROR: {
          std::cout << comalog_app_name << " [" << ANSI_COLOR_RED << std::string(log_level_str[level - 1]) << ANSI_COLOR_RESET <<"]: " << std::string(str);
          break;
        }
        case COMALOG_FATAL: {
          std::cout << comalog_app_name << " [" << ANSI_COLOR_MAGENTA << std::string(log_level_str[level - 1]) << ANSI_COLOR_RESET <<"]: " << std::string(str);
          break;
        }
        default: {
          std::cout << "Logging error! Check build environment, code and everything else! Panic!\n";
          break;
        }
        }
    #else
        std::cout << comalog_app_name << " [" << std::string(log_level_str[level - 1]) <<"]: " << std::string(str);
    #endif //LOG_USE_COLOURFUL_CONSOLE
#endif //COMALOG_DEBUG_PRINTS

        //va_end(argptr);
        //free(str); // !
#ifndef COMALOG_DEBUG_PRINTS
    }
#endif //COMALOG_DEBUG_PRINTS
}
