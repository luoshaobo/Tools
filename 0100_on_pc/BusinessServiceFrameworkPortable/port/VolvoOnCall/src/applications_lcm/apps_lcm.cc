///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file voc_lcm.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
//
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   Nov-27-2018
///////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <thread>
#include <chrono>

#include "applications_lcm/apps_lcm.h"
#include "voc_framework/signals/signal.h"
#include "signals/common/apps_lcm_signal.h"

#include "dlt/dlt.h"

extern "C"
{
#include "powermgr.h"
}

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call {

    std::function<void(LCMShutdownType)> AppsLcm::m_shutdown_cb_;
    std::function<void(std::shared_ptr<fsm::Signal>)> AppsLcm::m_resume_cb_;
    std::map<std::shared_ptr<fsm::VdServiceTransactionId>, bool> AppsLcm::m_apps_awake_map_;
    tpLCM_WakeupReason_t AppsLcm::m_wakeup_reason_;

    tpLCM_ReturnCode_t AppsLcm::ShutdownNotification(tpLCM_Shutdowntype_t shutdown_type)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s shutdownType: %d", __FUNCTION__, shutdown_type);
        tpLCM_AppInfo_t _appinfo;
        tpLCM_ReturnCode_t return_value = E_LCM_RET_FAILURE;
        (shutdown_type == E_LCM_SHUTDOWN) ? (DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, shutdown triggered: %d", __FUNCTION__, shutdown_type))
                : (DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, suspend triggered: %d", __FUNCTION__, shutdown_type));
        if( E_LCM_SUSPEND == shutdown_type ){
            m_shutdown_cb_(LCM_SUSPEND);
        } else {
            m_shutdown_cb_(LCM_SHUTDOWN);
        }

        std::strncpy(_appinfo.app_name, SERVICE_NAME, LCM_MAX_APPNAME_LENGTH);
        _appinfo.app_name[LCM_MAX_APPNAME_LENGTH - 1] = '\0';
        _appinfo.app_user_id = getuid();
        _appinfo.app_proc_id = getpid();
        _appinfo.app_thread_id = 0;
        return_value = tpLCM_shutdownReady(&_appinfo);
        if( E_LCM_RET_SUCCESS != return_value){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "AppsLcm::%s, tpLCM_shutdownReady error: %d", __FUNCTION__, return_value);
        }
        return E_LCM_RET_SUCCESS;
    }

    tpLCM_ReturnCode_t AppsLcm::ResumeToBroadcast(const tpLCM_WakeupReason_t wakeup_reason)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, wakeup_reason: %x", __FUNCTION__, wakeup_reason);
        m_wakeup_reason_ = wakeup_reason;
        fsm::VdServiceTransactionId vdsServiceTransactionId;
        std::shared_ptr<AppsLcmResumeSignal> _AppsLcmResumeSignal =
            std::make_shared<AppsLcmResumeSignal>(vdsServiceTransactionId, fsm::Signal::kAppsLcmResumeSignal, wakeup_reason);
        m_resume_cb_(_AppsLcmResumeSignal);
        return E_LCM_RET_SUCCESS;
    }

    AppsLcm::AppsLcm()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s", __FUNCTION__);
        memset((void*)&m_appinfo_, 0, sizeof(tpLCM_AppInfo_t));
    }

    AppsLcm::~AppsLcm()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s", __FUNCTION__);
    }

    void AppsLcm::KeepAwake(tpLCM_KeepAwakeReasons_t keep_awake_reason)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, keepAwakeReason: %d", __FUNCTION__, keep_awake_reason);
        tpLCM_ReturnCode_t ret_val;
        const tpLCM_KeepAwake_t keep_awake = {.appInfo = &m_appinfo_, .keepAwakeReason = keep_awake_reason};
        ret_val = tpLCM_keepAwakeReq(&keep_awake);
        if (ret_val != E_LCM_RET_SUCCESS) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "AppsLcm: %s: failed to request keep awake", __FUNCTION__);
        }
    }

    bool AppsLcm::IsWakeupReasonApps()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s", __FUNCTION__);
        tpLCM_WakeupReason_t wakeup_reason;
        tpLCM_ReturnCode_t ret_val;
        ret_val = tpLCM_getWakeupReason(&wakeup_reason);
        if (ret_val != E_LCM_RET_SUCCESS) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "AppsLcm: %s: failed to get wakeup-reason", __FUNCTION__);
            return false;
        }
        if (wakeup_reason == E_LCM_WAKEUP_SUBTYPE_ACC)
            return true;
        return false;
    }

    bool AppsLcm::Init(ShutdownCallback shutdown_cb, ResumeCallback resume_cb)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s", __FUNCTION__);
        tpLCM_ReturnCode_t ret_val;

        std::strncpy(m_appinfo_.app_name, SERVICE_NAME, LCM_MAX_APPNAME_LENGTH);
        m_appinfo_.app_name[LCM_MAX_APPNAME_LENGTH - 1] = '\0';
        m_appinfo_.app_user_id = getuid();
        m_appinfo_.app_proc_id = getpid();
        m_appinfo_.app_thread_id = 0;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"AppsLcm: %s: app_name:%s\t app_user_id:%d\t app_proc_id:%d\t app_thread_id : %d ", __FUNCTION__,
                m_appinfo_.app_name, m_appinfo_.app_user_id, m_appinfo_.app_proc_id, m_appinfo_.app_thread_id);

        ret_val = tpLCM_register(&m_appinfo_);
        if (ret_val != E_LCM_RET_SUCCESS) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,"AppsLcm: %s: tpLCM_register failed! ret value : %d",__FUNCTION__, ret_val);
            return false;
        }

        ret_val = tpLCM_registerShutdownSignal(ShutdownNotification);
        if (ret_val != E_LCM_RET_SUCCESS) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,"AppsLcm: %s: tpLCM_registerShutdownSignal failed! ret value : %d",__FUNCTION__, ret_val);
            return false;
        }

        ret_val = tpLCM_registerResumeSignal(ResumeToBroadcast);
        if (ret_val != E_LCM_RET_SUCCESS) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,"AppsLcm: %s: tpLCM_registerResumeSignal failed! ret value : %d",__FUNCTION__, ret_val);
            return false;
        }
        m_shutdown_cb_ = shutdown_cb;
        m_resume_cb_ = resume_cb;
        return true;
    }

    bool AppsLcm::pm_request_feature(std::shared_ptr<fsm::VdServiceTransactionId> transaction_id, bool keep_awake)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, operator: %s, map-key: 0x%x", __FUNCTION__, (keep_awake ? "keep awake" : "sleep"), transaction_id.get());
        int map_size = m_apps_awake_map_.size();
        Apps_Awake_Map_iter search_iter = m_apps_awake_map_.find(transaction_id);
        if(keep_awake){
            if(0 == map_size)
                KeepAwake(E_LCM_KEEPAWAKE_NO_BUS);
            if (m_apps_awake_map_.end() == search_iter){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, inster to map, map-key: 0x%x", __FUNCTION__, transaction_id.get());
                m_apps_awake_map_.insert(std::make_pair(transaction_id, keep_awake));
            }
        } else {
            if(m_apps_awake_map_.end() == search_iter){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, m_apps_awake_map_ not find transaction_id adds: 0x%x", __FUNCTION__, transaction_id.get());
                return false;
            } else {
                m_apps_awake_map_.erase(search_iter);
                if(0 == m_apps_awake_map_.size()){
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, sleep transaction_id adds: 0x%x", __FUNCTION__, transaction_id.get());
                    KeepAwake(E_LCM_KEEPAWAKE_IDLE);
                }
            }
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, m_apps_awake_map_.size: %d", __FUNCTION__, m_apps_awake_map_.size());
        return true;
    }

    bool AppsLcm::RequestAwakeRTC(vc::Req_RTCAwakeVuc req)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        if (nullptr == vehicle_comm_request_id.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, nullptr == vehicle_comm_request_id", __FUNCTION__);
            return false;
        }
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        if (igen.Request_RTCAwakeVuc(&req, vehicle_comm_request_id->GetId()) == vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, send Request_RTCAwakeVuc success", __FUNCTION__);
            ret = true;
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s, send Request_RTCAwakeVuc failed", __FUNCTION__);
        }
        return ret;
    }

    AppsLcm& AppsLcm::GetInstance()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "AppsLcm::%s", __FUNCTION__);
        static AppsLcm instance;
        return instance;
    }
}