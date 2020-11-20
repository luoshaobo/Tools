#ifndef VC_LOG_HPP
#define VC_LOG_HPP

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

/** @file vc_log.hpp
 * This file provides functionality for printing in VehicleComm.
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

// Collide with C-code defines in DLT (not in a namespace)
#undef LOG_ERR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG

namespace vc
{
    enum LogLevel {
        LOG_DEFAULT,
        LOG_OFF,
        LOG_FATAL,
        LOG_ERR,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG,
        LOG_VERB
    };

    const char *const vclog_level_str[] = {"DEFAULT", "OFF  ", "FATAL", "ERR  ", "WARN ", "INFO ", "DEBUG", "VERB "};

    void LOG(LogLevel level, const char *format, ...);
    void LOG_INIT(const char *name, const char *description);
    void LOG_DEINIT();

} // namespace vc

#endif // VC_LOG_HPP
