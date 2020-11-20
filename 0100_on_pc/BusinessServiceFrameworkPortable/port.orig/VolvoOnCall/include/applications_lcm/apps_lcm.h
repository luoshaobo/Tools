///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file apps_lcm.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
//
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   Nov-27-2018
///////////////////////////////////////////////////////////////////
#ifndef GLY_VOC_LCM_H_
#define GLY_VOC_LCM_H_

#include <functional>

extern "C" {
#include "lifecycle_service.h"
#include "tpsys.h"
}
#include <map>
#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/signal.h"

#include "vehicle_comm_messages.hpp"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include <condition_variable>
#include <mutex>
#include <deque>
#include <thread>

namespace volvo_on_call{

#define SERVICE_NAME "Apps"
typedef std::map<std::shared_ptr<fsm::VdServiceTransactionId>, bool> Apps_Awake_Map;
typedef Apps_Awake_Map::iterator Apps_Awake_Map_iter;


typedef enum {
    LCM_SUSPEND,
    LCM_SHUTDOWN
} LCMShutdownType;

const char* const LCMShutdownTypeStr[] = {
    "LCM_SUSPEND",
    "LCM_SHUTDOWN"
};

typedef std::function<void(LCMShutdownType)> ShutdownCallback;
typedef std::function<void(std::shared_ptr<fsm::Signal>)> ResumeCallback;

typedef std::deque<std::shared_ptr<fsm::Signal>> ResumeSignalDeque;
/**
    @brief handle of the OTP lifecycle manager
    
    This class is handling all communication with the lifecycle manager and is reporting
    different states back to class that initiates it with its callback.
*/
class AppsLcm{
private:
    tpLCM_AppInfo_t m_appinfo_;
    static ShutdownCallback m_shutdown_cb_;
    static ResumeCallback m_resume_cb_;
    static Apps_Awake_Map m_apps_awake_map_;

    static tpLCM_WakeupReason_t m_wakeup_reason_;

    ////////////////////////////////////////////////////////////
    // @brief : ShutdownNotification
    //
    // @param[in]  tpLCM_Shutdowntype_t
    // @return     tpLCM_ReturnCode_t
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    static tpLCM_ReturnCode_t ShutdownNotification(tpLCM_Shutdowntype_t shutdown_type);

    ////////////////////////////////////////////////////////////
    // @brief : ResumeToBroadcast
    //
    // @param[in]  tpLCM_WakeupReason_t
    // @return     tpLCM_ReturnCode_t
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    static tpLCM_ReturnCode_t ResumeToBroadcast(const tpLCM_WakeupReason_t wakeup_reason);

    ////////////////////////////////////////////////////////////
    // @brief : AppsLcm Constructor for the VocLcm-class.
    //
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    AppsLcm();

    ////////////////////////////////////////////////////////////
    // @brief : ~AppsLcm Destructor for the VocLcm-class.
    //
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    ~AppsLcm();

    ////////////////////////////////////////////////////////////
    // @brief : KeepAwaker Send a keep awake request for APPs
    //
    // @param[in] keep_awake_reason
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    void KeepAwake(tpLCM_KeepAwakeReasons_t keep_awake_reason);

public:
    ////////////////////////////////////////////////////////////
    // @brief : IsWakeupReasonVoc Check if Apps is the wake up reason
    //
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    bool IsWakeupReasonApps();

    ////////////////////////////////////////////////////////////
    // @brief : Init
    //
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    bool Init(ShutdownCallback shutdown_cb, ResumeCallback resume_cb);

    ////////////////////////////////////////////////////////////
    // @brief : interface: Send keep awake request for APPs
    //
    // @param[in] transaction_id as a map key
    // @param[in] keep_awake, true: keep awake; false: sleep
    // @return[in] bool
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    bool pm_request_feature(std::shared_ptr<fsm::VdServiceTransactionId> transaction_id, bool keep_awake);

    ////////////////////////////////////////////////////////////
    // @brief : interface:  awake Vuc in timer throught RTC of Vuc
    //
    // @param[in] 
    // @return[in] bool
    // @author     uia93888, Dec 12, 2018
    ///////////////////////////////////////////////////////////
    bool RequestAwakeRTC(vc::Req_RTCAwakeVuc req);

    ////////////////////////////////////////////////////////////
    // @brief : GetAwakeReason return the awake reason
    //
    // @return[in] tpLCM_WakeupReason_t
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    tpLCM_WakeupReason_t GetAwakeReason() { return m_wakeup_reason_; }

    ////////////////////////////////////////////////////////////
    // @brief : GetInstance Get the singleton instance of VocLcm
    //
    // @author     uia93888, Nov 27, 2018
    ///////////////////////////////////////////////////////////
    static AppsLcm& GetInstance();
};
}



#endif //GLY_VOC_LCM_H_