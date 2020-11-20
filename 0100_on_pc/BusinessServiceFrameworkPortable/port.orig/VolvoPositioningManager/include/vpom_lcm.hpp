/*
* Copyright (C) 2018 Continental Automotive GmbH
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
 *
 * @file vpom_lcm.hpp
 * @brief This file handles the lifecycle manager functionality for VolvoPositioningMananger
 *
 *  \addtogroup volvo_positioning_manager
 *  \{
 */

#ifndef VPOM_LCM_HPP
#define VPOM_LCM_HPP

#include "dlt/dlt.h"
#include <functional>
#include <thread>

extern "C" {
#include "lifecycle_service.h"
#include "tpsys.h"
}

#define SERVICE_NAME "vpom"       ///< VPOM service name

#define UNUSED_VAR(x) (void)(x)

typedef enum {
    VPOM_LCM_SHUTDOWN,
    VPOM_LCM_SUSPEND,
    VPOM_LCM_RESUME
} VpomLcmSignal;

/**
    @brief Handling of the OTP lifecycle manager.

    This class is handling all communication with the lifecycle manager and is reporting
    different states back to class that initiates it with its callback.
*/
class VpomLCM {
private:
    VpomLCM(); // Prevent construction
    VpomLCM(const VpomLCM&) = delete; // Prevent construction by copying
    VpomLCM& operator=(const VpomLCM&) = delete; // Prevent assignment
    ~VpomLCM(); // Prevent unwanted destruction

    //External Callback functions
    static tpLCM_ReturnCode_t ShutdownNotification(tpLCM_Shutdowntype_t shutdown_type); /**< The callback that LCM framework calls */
    static tpLCM_ReturnCode_t ResumeNotification(const tpLCM_WakeupReason_t wakeup_reason); /**< The callback that LCM framework calls */

    //Internal Callback functions
    tpLCM_ReturnCode_t ShutdownNotification_(tpLCM_Shutdowntype_t shutdown_type);
    tpLCM_ReturnCode_t ResumeNotification_(const tpLCM_WakeupReason_t wakeup_reason);

    std::function<int(VpomLcmSignal)> vpomLcmHandler_;
    std::thread tmpThr_;

public:
    bool Init(std::function<int(VpomLcmSignal)> vpomLcmHandler);
    static VpomLCM& getInstance();
};

#endif //VPOM_LCM_HPP

/** \}    end of addtogroup */
