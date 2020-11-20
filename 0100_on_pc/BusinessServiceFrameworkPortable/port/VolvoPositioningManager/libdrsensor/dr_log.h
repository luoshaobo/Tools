/*******************************************************************************
*
*  Copyright 2017 Continental Automotive Systems Inc.
*
*  Filename     : dr_log.h
*
*  Created by   : Andrei Varvaruc
*
*  Date Created : 24-Oct-2017
*
*  Description  : DR logging macros
*
*****************************************************************************/
#ifndef __DR_LOG_H__
#define __DR_LOG_H__

#include <sys/syslog.h>

#define DR_PREFIX "CONTI-DR: %s:"

#define DR_LOG_ERR(format, ...)	      syslog(LOG_ERR, DR_PREFIX format, __func__, ##__VA_ARGS__)
#define DR_LOG_WARN(format, ...)      syslog(LOG_WARNING, DR_PREFIX format, __func__, ##__VA_ARGS__)
#define DR_LOG_INFO(format, ...)	  syslog(LOG_INFO, DR_PREFIX format, __func__, ##__VA_ARGS__)

#if defined (DISABLE_DEBUG_LOG)
    #define DR_LOG_DEBUG(format, ...)
#else
    #define DR_LOG_DEBUG(format, ...) syslog(LOG_DEBUG, DR_PREFIX format, __func__, ##__VA_ARGS__)
#endif

#endif	// __DR_LOG_H__