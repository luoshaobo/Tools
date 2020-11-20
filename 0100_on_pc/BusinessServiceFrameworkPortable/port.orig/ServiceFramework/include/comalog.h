/**
 * Copyright (C) 2016 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

/**
 * @file       comalog.h
 * @project    VCC TCAM
 * @subsystem  CommunicationManager
 * @version    0.1
 * @date       20170206
 * @brief      Providing a means of logging output to DLT and stdout.
 * @author     Bernd Winter
 *
 * @note       derived from VehicleComm's logging setup
 *
 * @par Module-History:
 * @verbatim
 *  Date       Author                  Reason
 *  20170207   Bernd Winter            init
 *
 * @endverbatim
 *
 */

#ifndef COMALOG_H__
#define COMALOG_H__

#include "operatingsystem.h"
#include <string>

#ifndef COMALOG_NO_DLT
#include "dlt.h"
#endif

#if defined(COMALOG_NO_DLT) && !defined(COMALOG_DEBUG_PRINTS)
#define COMALOG_DEBUG_PRINTS
#endif

// for a fancy development console output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

enum ComaLogLevel
{
	COMALOG_FATAL = 1, COMALOG_ERROR = 2, COMALOG_WARN = 3, COMALOG_INFO = 4, COMALOG_DEBUG = 5, COMALOG_VERBOSE = 6
};

const char * const log_level_str[] =
{ "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "VERB " };

//extern char comalog_app_name[];
Lib_API void COMALOG(ComaLogLevel level, const char *format, ...);
Lib_API void COMALOG_INIT(const char *app_name, const char *description);
Lib_API void COMALOG_DEINIT();

#endif //COMALOG_H__
