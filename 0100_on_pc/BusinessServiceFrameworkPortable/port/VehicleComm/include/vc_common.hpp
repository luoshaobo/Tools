#ifndef VC_COMMON_HPP
#define VC_COMMON_HPP

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

/** @file vc_common.hpp
 * This file implements types and classes used by most parts of VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        07-Dec-2016
 ***************************************************************************/

#include "vc_log.hpp"
#include "vehicle_comm_types.hpp"
#include "vehicle_comm_messages.hpp"

namespace vc {

/**
    @brief VC System events
*/
typedef enum {
    SYSEVENT_STARTED,
} SystemEvent;

const char *const SystemEventStr[] = {
    "SYSEVENT_STARTED",
};

/**
    @brief Mapping of a request-ID to a destinination
*/
struct VCRequestDestination {
    RequestID request_id; /**< The request-ID */
    Endpoint dst;         /**< The destination, inside VC, for messages with this request-ID */
};

} // namespace vc

#endif // VC_COMMON_HPP
