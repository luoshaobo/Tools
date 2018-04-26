#ifndef LOG_H_7832894109072348912040257818949823578209402385278348928945345345
#define LOG_H_7832894109072348912040257818949823578209402385278348928945345345

#define MS_LOG_PREFIX               "[MS] "

#define MS_LOG_LEVEL_DEBUG           0
#define MS_LOG_LEVEL_INFO            1
#define MS_LOG_LEVEL_WARNING         2
#define MS_LOG_LEVEL_ERROR           3
#define MS_LOG_LEVEL_CRITICAL        4
#define MS_LOG_LEVEL_FATAL           5

#ifdef WIN32
#define MS_DEBUG(format,...)         MS_Log(MS_LOG_LEVEL_DEBUG, format, __VA_ARGS__)
#define MS_INFO(format,...)          MS_Log(MS_LOG_LEVEL_INFO, format, __VA_ARGS__)
#define MS_WARNING(format,...)       MS_Log(MS_LOG_LEVEL_WARNING, format, __VA_ARGS__)
#define MS_ERROR(format,...)         MS_Log(MS_LOG_LEVEL_ERROR, format, __VA_ARGS__)
#define MS_CRITICAL(format,...)      MS_Log(MS_LOG_LEVEL_CRITICAL, format, __VA_ARGS__)
#define MS_FATAL(format,...)         MS_Log(MS_LOG_LEVEL_FATAL, format, __VA_ARGS__)
#else // #ifdef WIN32
#define MS_DEBUG(format,...)         MS_Log(MS_LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
#define MS_INFO(format,...)          MS_Log(MS_LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define MS_WARNING(format,...)       MS_Log(MS_LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define MS_ERROR(format,...)         MS_Log(MS_LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define MS_CRITICAL(format,...)      MS_Log(MS_LOG_LEVEL_CRITICAL, format, ## __VA_ARGS__)
#define MS_FATAL(format,...)         MS_Log(MS_LOG_LEVEL_FATAL, format, ## __VA_ARGS__)
#endif // #ifdef WIN32

#define MS_CURFUNC                   __FUNCTION__

void MS_Log(int nLogLevel, const char *pFormat, ...);

#define MS_LOG_FUNC()               MS_DEBUG("%s() in line [%d]", MS_CURFUNC, __LINE__)

#endif // #ifndef LOG_H_7832894109072348912040257818949823578209402385278348928945345345
