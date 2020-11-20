///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file gly_vds_apps_lcm_signal.h
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   Nov-27-2018
///////////////////////////////////////////////////////////////////
#ifndef APPS_VEHICLE_LCM_RESUME_SIGNAL_H_
#define APPS_VEHICLE_LCM_RESUME_SIGNAL_H_
extern "C" {
#include "lifecycle_service.h"
}
#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/transactions/transaction_id.h"

#include <memory>
#include <string>

namespace volvo_on_call {
    
    class AppsLcmResumeSignal: public fsm::Signal
    {
        public:
            AppsLcmResumeSignal(fsm::VdServiceTransactionId &vdsId, const long signal_type, const tpLCM_WakeupReason_t is_apps_wakeup_reason):
                fsm::Signal::Signal(vdsId,signal_type),
                m_is_apps_wakeup_reason(is_apps_wakeup_reason)
            {}
            ~AppsLcmResumeSignal(){};
            tpLCM_WakeupReason_t GetIsAppsWakeupReason(){ return m_is_apps_wakeup_reason; }
            std::string ToString(){ return "kAppsLcmResumeSignal"; }
        private:
            tpLCM_WakeupReason_t m_is_apps_wakeup_reason;
    };
}



#endif //APPS_VEHICLE_LCM_RESUME_SIGNAL_H_