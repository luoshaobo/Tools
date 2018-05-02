#ifndef HMI_UTIL_H
#define HMI_UTIL_H

#define HMI_LOG_LEVEL_DEBUG           0
#define HMI_LOG_LEVEL_INFO            1
#define HMI_LOG_LEVEL_WARNING         2
#define HMI_LOG_LEVEL_ERROR           3
#define HMI_LOG_LEVEL_CRITICAL        4
#define HMI_LOG_LEVEL_FATAL           5

////////////////////////////////////////////////////////////////////////////////////////////////////

#define HMI_RADIO_LOG_PREFIX               "[HMI_RADIO] "

#define HMI_RADIO_DEBUG(format,...)         HMI_Log(HMI_RADIO_LOG_PREFIX, HMI_LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
#define HMI_RADIO_INFO(format,...)          HMI_Log(HMI_RADIO_LOG_PREFIX, HMI_LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define HMI_RADIO_WARNING(format,...)       HMI_Log(HMI_RADIO_LOG_PREFIX, HMI_LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define HMI_RADIO_ERROR(format,...)         HMI_Log(HMI_RADIO_LOG_PREFIX, HMI_LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define HMI_RADIO_CRITICAL(format,...)      HMI_Log(HMI_RADIO_LOG_PREFIX, HMI_LOG_LEVEL_CRITICAL, format, ## __VA_ARGS__)
#define HMI_RADIO_FATAL(format,...)         HMI_Log(HMI_RADIO_LOG_PREFIX, HMI_LOG_LEVEL_FATAL, format, ## __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////////////////////////

#define HMI_MEDIA_LOG_PREFIX               "[HMI_MEDIA] "

#define HMI_MEDIA_DEBUG(format,...)         HMI_Log(HMI_MEDIA_LOG_PREFIX, HMI_LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
#define HMI_MEDIA_INFO(format,...)          HMI_Log(HMI_MEDIA_LOG_PREFIX, HMI_LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define HMI_MEDIA_WARNING(format,...)       HMI_Log(HMI_MEDIA_LOG_PREFIX, HMI_LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define HMI_MEDIA_ERROR(format,...)         HMI_Log(HMI_MEDIA_LOG_PREFIX, HMI_LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define HMI_MEDIA_CRITICAL(format,...)      HMI_Log(HMI_MEDIA_LOG_PREFIX, HMI_LOG_LEVEL_CRITICAL, format, ## __VA_ARGS__)
#define HMI_MEDIA_FATAL(format,...)         HMI_Log(HMI_MEDIA_LOG_PREFIX, HMI_LOG_LEVEL_FATAL, format, ## __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////////////////////////

void HMI_Log(const char *prefix, int nLogLevel, const char *pFormat, ...);

#endif // HMI_UTIL_H
