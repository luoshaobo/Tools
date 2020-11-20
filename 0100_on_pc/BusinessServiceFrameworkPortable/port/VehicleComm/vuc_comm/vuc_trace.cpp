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

/** @file vuc_trace.cpp
 * This file provides functionality for printing in vuc traces.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include <iostream>
#include <stdarg.h>

#include "vuc_trace.hpp"

#ifndef VCLOG_NO_DLT
DLT_DECLARE_CONTEXT(VUCTRACE_DLT_CONTEXT); //DLT_CONTEXT set in compile time see CMakeList.txt file
#endif

namespace vc {

void VUCTRACE(VuCTraceLevel level, const char *format, ...)
{
    #ifndef VCLOG_NO_DLT
        #ifndef VCLOG_DEBUG_PRINTS
            if(level <= DLT_LOG_INFO) {
        #endif
            char* str;
            va_list args;
            va_start(args, format);
            vasprintf(&str, format, args);
            int lvl = level-1;
            DLT_LOG_STRING(VUCTRACE_DLT_CONTEXT, (DltLogLevelType)lvl, str);
            va_end(args);
            free(str);
        #ifndef VCLOG_DEBUG_PRINTS
        }
        #endif
    #endif
}

void VUCTRACE_INIT(const char *name, const char *description)
{
    #ifndef VCLOG_NO_DLT
        #ifdef VCLOG_DEBUG_PRINTS
        DLT_REGISTER_CONTEXT_LL_TS(VUCTRACE_DLT_CONTEXT, name, description, DLT_LOG_VERBOSE, DLT_TRACE_STATUS_ON);
        #else
        DLT_REGISTER_CONTEXT(VUCTRACE_DLT_CONTEXT, name, description);
        #endif //VCLOG_DEBUG_PRINTS
    #endif //VCLOG_NO_DLT
}

void VUCTRACE_DEINIT()
{
    #ifndef VCLOG_NO_DLT
    DLT_UNREGISTER_CONTEXT(VUCTRACE_DLT_CONTEXT);
    DLT_UNREGISTER_APP();
    #endif //VCLOG_NO_DLT
}

} //namespace vc
