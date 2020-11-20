#ifndef VUC_TRACE_HPP
#define VUC_TRACE_HPP
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

/** @file vuc_trace.hpp
 * This file provides functionality for printing in vuc traces.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/
#ifndef VCLOG_NO_DLT
#include "dlt.h"
#include "dlt_user.h"
#include "dlt_user_macros.h"
#endif

#if defined(VCLOG_NO_DLT) && !defined(VCLOG_DEBUG_PRINTS)
#define VCLOG_DEBUG_PRINTS
#endif

namespace vc
{
    enum VuCTraceLevel {
        VUCTRACE_DEFAULT,
        VUCTRACE_OFF,
        VUCTRACE_FATAL,
        VUCTRACE_ERR,
        VUCTRACE_WARN,
        VUCTRACE_INFO,
        VUCTRACE_DEBUG,
        VUCTRACE_VERB
    };

    const char *const vuclog_message_name_str[] = {"TRACE_ENTRY", "TRACE_CONFIG_IPC", "TRACE_CONTROL", "TRACE_CONFIG_CAN"};
    const char *const vuclog_type_str[] = {"REPORT", "RESPONSE", "REQUEST", "UNUSED"};
    const char *const vuclog_trace_module_str[] = { "UNUSED", "CANH", "BLEH", "KLBH", "A2BD",
                                                    "DIAG", "IPC", "BUB", "CPU", "RF", "PWR", "ALCH", "DTCH",
                                                    "ABAL", "TRCH", "AUAD", "ACCH" };

    void VUCTRACE(VuCTraceLevel level, const char *format, ...);
    void VUCTRACE_INIT(const char *name, const char *description);
    void VUCTRACE_DEINIT();
} //namespace vc

#endif //VUC_TRACE_HPP
