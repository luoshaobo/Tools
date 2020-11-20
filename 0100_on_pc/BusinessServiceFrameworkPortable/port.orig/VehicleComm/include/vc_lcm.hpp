#ifndef VC_LCM_HPP
#define VC_LCM_HPP

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

/** @file vc_lcm.hpp
 * This file handles the lifecycle manager functionality for VehicleComm.
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Johan Bohlin & Ola Lilja
 * @date        28-Dec-2016
 ***************************************************************************/

#include <functional>

extern "C" {
#include "lifecycle_service.h"
#include "tpsys.h"
}

#include "vc_common.hpp"

#define SERVICE_NAME "vc"

namespace vc {

typedef enum {
    VCLCM_RESUME,
    VCLCM_SUSPEND,
    VCLCM_SHUTDOWN,
} VCLCMSignal;


/**
    @brief Handling of the OTP lifecycle manager.

    This class is handling all communication with the lifecycle manager and is reporting
    different states back to class that initiates it with its callback.
*/
class LCM {
private:
    static std::function<ReturnValue(VCLCMSignal)> lcmhandler_; /**< The callback registered by user for events */
    static tpLCM_ReturnCode_t ShutdownNotification(tpLCM_Shutdowntype_t shutdown_type); /**< The callback that LCM framework calls */
    static tpLCM_ReturnCode_t ResumeNotification(const tpLCM_WakeupReason_t wakeup_reason); /**< The callback that LCM framework calls */
    LCM(); // Prevent construction
    LCM(const LCM&); // Prevent construction by copying
    LCM& operator=(const LCM&); // Prevent assignment
    ~LCM(); // Prevent unwanted destruction

public:
    ReturnValue Init(std::function<ReturnValue(VCLCMSignal)> lcmhandler);
    static LCM& GetInstance();
};

} // namespace vc

#endif //VC_LCM_HPP
