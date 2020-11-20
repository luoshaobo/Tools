///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file svt.h
//	VOC Service car svt request transaction.

// @project		GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date	1-Aug-2018
///////////////////////////////////////////////////////////////////

#include <time.h>
#include <stdio.h>
#include "dlt/dlt.h"
#include "vpom_IPositioningService.hpp"

#include "svt/svt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_svt_signal.h"
#include "voc_framework/signals/vpom_signal.h"
#include "voc_framework/signals/internal_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vpom_signal_source.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include "../voc_persist_data_mgr/include/voc_persist_data_mgr.h"
#include "applications_lcm/apps_lcm.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

Svt::Svt() :
    fsm::SmartTransaction(kNew),
    m_vocpersist(vocpersistdatamgr::VocPersistDataMgr::GetInstance()),
    m_execteTime(0)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s", __FUNCTION__);

    SignalFunction enqueue_svt_request =
        std::bind(&Svt::EnqueueSvtRequest,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_vehicle_comm =
        std::bind(&Svt::HandleVehicleComm,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_svt_restart =
        std::bind(&Svt::HandleSvtReStart,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_start_state =
        std::bind(&Svt::HandleStartState,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_period_state =
        std::bind(&Svt::HandlePeriodState,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_stop_state =
        std::bind(&Svt::HandleStopState,
                  this,
                  std::placeholders::_1);

    SignalFunction handle_position =
        std::bind(&Svt::HandlePosition,
                  this,
                  std::placeholders::_1);
    
    StateFunction handle_done_state =
        std::bind(&Svt::HandleDoneState,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2);
    
    SignalFunction handle_timeout =
        std::bind(&Svt::HandleTimeout,
                  this,
                  std::placeholders::_1);
    
    SignalFunction handle_AppsLcmResume =
        std::bind(&Svt::HandleAppsLcmResumeSignal,
                  this,
                  std::placeholders::_1);

    StateMap state_map =
        {{kNew,                      {nullptr,
                                      {{fsm::kVDServiceSvt, enqueue_svt_request},
                                       {fsm::Signal::kCarMode, handle_vehicle_comm},
                                       {fsm::Signal::kStolenVehicleTracking, handle_vehicle_comm},
                                       {fsm::Signal::kTimeout, handle_timeout},
                                       {fsm::Signal::kAppsLcmResumeSignal, handle_AppsLcmResume},
                                       {VocInternalSignalTypes::kSvtReStart, handle_svt_restart}},
                                      {kStart, kStop, kDone}}},

         {kStart,                    {nullptr,
                                      {{fsm::kVDServiceSvt, enqueue_svt_request},
                                       {fsm::Signal::kCarMode, handle_vehicle_comm},
                                       {fsm::Signal::kStolenVehicleTracking, handle_vehicle_comm},
                                       {fsm::Signal::kTimeout, handle_timeout},
                                       {fsm::Signal::kAppsLcmResumeSignal, handle_AppsLcmResume},
                                       {VocInternalSignalTypes::kSvtStart, handle_start_state}},
                                      {kStart, kPeriod, kStop, kDone}}},

         {kPeriod,                    {nullptr,
                                      {{fsm::kVDServiceSvt, enqueue_svt_request},
                                       {fsm::Signal::kCarMode, handle_vehicle_comm},
                                       {fsm::Signal::kStolenVehicleTracking, handle_vehicle_comm},
                                       {fsm::Signal::kTimeout, handle_timeout},
                                       {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                                       {fsm::Signal::kAppsLcmResumeSignal, handle_AppsLcmResume},
                                       {VocInternalSignalTypes::kSvtPeriod, handle_period_state}},
                                      {kPeriod, kStop, kDone}}},

         {kStop,                     {nullptr,
                                      {{fsm::kVDServiceSvt, enqueue_svt_request},
                                       {fsm::Signal::kCarMode, handle_vehicle_comm},
                                       {fsm::Signal::kStolenVehicleTracking, handle_vehicle_comm},
                                       {fsm::Signal::kTimeout, handle_timeout},
                                       {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                                       {fsm::Signal::kAppsLcmResumeSignal, handle_AppsLcmResume},
                                       {VocInternalSignalTypes::kSvtStop, handle_stop_state}},
                                      {kStop, kDone}}},
                                      
         {kDone,                     {handle_done_state,
                                      SignalFunctionMap(),
                                      {}}}};

    SetStateMap(state_map);

    MapSignalType(VocInternalSignalTypes::kSvtReStart);
    MapSignalType(VocInternalSignalTypes::kSvtStart);
    MapSignalType(VocInternalSignalTypes::kSvtPeriod);
    MapSignalType(VocInternalSignalTypes::kSvtStop);
    MapSignalType(fsm::Signal::kAppsLcmResumeSignal);

    LoadPersistData();
    
    /*if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }*/
    m_SvtLcmTransactionId_ = std::make_shared<fsm::VdServiceTransactionId>();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, m_SvtLcmTransactionId_ adds: 0x%x", __FUNCTION__, m_SvtLcmTransactionId_.get());
}


Svt::~Svt  ()
{
     DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"Svt::%s\n", __FUNCTION__);
}

bool Svt::LoadPersistData()
{
    //get wait for gps time
    bool result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::SVT_WAIT_FOR_GPS_TIME, m_waitForGPSTime);
    if(!result)
    {
        m_waitForGPSTime = kWaitForGPSTime;
    }

    return true;
}
    
bool Svt::EnqueueSvtRequest(std::shared_ptr<fsm::Signal> signal)
{
    State state = GetState();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current state:%d\n", __FUNCTION__,state);
    
    SignalWithStatus_t signalStatus;
    signalStatus.status = UNCHECK;
    signalStatus.signal = std::dynamic_pointer_cast<GlyVdsSvtSignal>(signal);
    EnqueueSvtSignal(signalStatus);
    
    TriggerNextSvtSignal();
    
    return true;
}


bool Svt::HandleSvtRequest(SignalWithStatus_t signalStatus)
{

    State state = GetState();
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current state:%d\n", __FUNCTION__,state);
    
    std::shared_ptr<SvtRequest_t> request = signalStatus.signal->GetRequestPayload();
    
    switch(request->serviceCommand)
    {
        case startService:
            
            CheckStartCondition(signalStatus);
            
            break;
        case terminateService:
            
            CheckStopCondition(signalStatus);
            
            break;
        default:
            
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "tsp service command is NOK:%d,send invalidData ack\n", request->serviceCommand);
            SendEndAck(signalStatus,invalidData);
            
            break;
    }


    return true;
}


bool Svt::HandleSvtReStart(std::shared_ptr<fsm::Signal> signal)
{
    return true;
}

bool Svt::HandleStartState(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);

    SignalWithStatus_t runningSignal;
    bool result = GetSvtSignal(RUNNING,&runningSignal);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current has not running signal.\n", __FUNCTION__);
        TriggerNextSvtSignal();
        return false;
    }
    
    std::shared_ptr<SvtRequest_t> request = runningSignal.signal->GetRequestPayload();
    
    Timestamp::TimeVal utcTime = GetUtcTime();
    long int startIntervalTmp = request->scheduledTimeSeconds - (utcTime/1000000);

    std::lock_guard<std::mutex> lock(m_request_timeout_mutex);
    
    RemoveTimeout(m_kSvtStart_tick_id);
    m_kSvtStart_tick_id = fsm::TimeoutTransactionId();
    MapTransactionId(m_kSvtStart_tick_id.GetSharedCopy());
    RequestTimeout(m_kSvtStart_tick_id,static_cast<std::chrono::seconds>(startIntervalTmp), false);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Timer interval timeOut %lds started,m_kSvtStart_tick_id:%lld\n", startIntervalTmp, m_kSvtStart_tick_id.GetId());
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, m_SvtLcmTransactionId_ adds: 0x%x", __FUNCTION__, m_SvtLcmTransactionId_.get());
    if ( !AppsLcm::GetInstance().pm_request_feature(m_SvtLcmTransactionId_, true))
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, operate pm_request_feature(true) failed\n", __FUNCTION__);
        
    return true;
}

bool Svt::HandlePeriodState(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);

    SignalWithStatus_t runningSignal;
    bool result = GetSvtSignal(RUNNING,&runningSignal);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current has not running signal.\n", __FUNCTION__);
        TriggerNextSvtSignal();
        return false;
    }
    
    std::shared_ptr<SvtRequest_t> request = runningSignal.signal->GetRequestPayload();
    
    Timestamp::TimeVal utcTime = GetUtcTime();
    long int interval = request->interval;
    long int endIntervalTmp = request->endTimeSeconds - (utcTime/1000000);
    m_execteTime = request->endTimeSeconds;
    
    /*if (GLY_APP_SVT_PREDEFINED_TIMER < interval){
        if ( !AppsLcm::GetInstance().pm_request_feature(m_SvtLcmTransactionId_, false) ){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, operate pm_request_feature(false) failed\n", __FUNCTION__);
        }
    }*/
    
    if(endIntervalTmp < interval)
        interval = endIntervalTmp;
    
    std::lock_guard<std::mutex> lock(m_request_timeout_mutex);
    
    RemoveTimeout(m_kSvtPeriod_tick_id);
    m_kSvtPeriod_tick_id = fsm::TimeoutTransactionId();
    MapTransactionId(m_kSvtPeriod_tick_id.GetSharedCopy());
    RequestTimeout(m_kSvtPeriod_tick_id, static_cast<std::chrono::seconds>(interval), true);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Timer interval timeOut %lds send data,m_kSvtPeriod_tick_id:%lld\n", interval,m_kSvtPeriod_tick_id.GetId());
        
    RemoveTimeout(m_kSvtStop_tick_id);
    m_kSvtStop_tick_id = fsm::TimeoutTransactionId();
    MapTransactionId(m_kSvtStop_tick_id.GetSharedCopy());
    RequestTimeout(m_kSvtStop_tick_id, static_cast<std::chrono::seconds>(endIntervalTmp), false);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Timer endIntervalTmp timeOut %lds send data,m_kSvtStop_tick_id:%lld\n", endIntervalTmp,m_kSvtStop_tick_id.GetId());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, interval: %ds, predefined time: %ds\n", __FUNCTION__, GLY_APP_SVT_PREDEFINED_TIMER);
    return true;
}

bool Svt::HandleStopState(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    
    SignalWithStatus_t runningSignal;
    bool result = GetSvtSignal(RUNNING,&runningSignal);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current has not running signal.\n", __FUNCTION__);
        TriggerNextSvtSignal();
        return false;
    }
    
    std::shared_ptr<SvtStopSignal> svtStopSignal = std::dynamic_pointer_cast<SvtStopSignal>(signal);
    if(svtStopSignal->getSendData())
    {
        RequestPosition(nullptr,true);
    }
    
    if(svtStopSignal->getCanStop())
    {
        SendEndAck(runningSignal,-1);
        
        RemoveMyTimeouts();
        RemoveAllWaitForGPSTicks();
        
        runningSignal.status = COMPLETE;
        UpdateSvtSignal(runningSignal);

        TriggerNextSvtSignal();
    }
    
    return true;
}

bool Svt::HandleDoneState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,svt finished.\n", __FUNCTION__);
    
    Stop();
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, m_SvtLcmTransactionId_ adds: 0x%x", __FUNCTION__, m_SvtLcmTransactionId_.get());
    if ( !AppsLcm::GetInstance().pm_request_feature(m_SvtLcmTransactionId_, false) ){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, operate pm_request_feature(false) failed\n", __FUNCTION__);
    }
    return true;
}

bool Svt::HandleTimeout(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    
    if(signal->GetTransactionId() == m_kSvtStart_tick_id
        || signal->GetTransactionId() == m_kSvtPeriod_tick_id)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, m_SvtLcmTransactionId_ adds: 0x%x", __FUNCTION__, m_SvtLcmTransactionId_.get());
        if ( !AppsLcm::GetInstance().pm_request_feature(m_SvtLcmTransactionId_, true) ){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, operate pm_request_feature(true) failed\n", __FUNCTION__);
        }
        RequestPosition();
        
        if(signal->GetTransactionId() == m_kSvtStart_tick_id)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "next state is period.\n");
            SetState(kPeriod);
            std::shared_ptr<fsm::Signal> svtPeriodSignal = std::make_shared<SvtPeriodSignal>();
            EnqueueSignal(svtPeriodSignal);
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "current state is period send data.\n");
        }
    }
    else if(signal->GetTransactionId() == m_kSvtStop_tick_id)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "next state is stop.\n");
        RemoveTimeout(m_kSvtPeriod_tick_id);
        SetState(kStop);
        std::shared_ptr<SvtStopSignal> svtStopSignal = std::make_shared<SvtStopSignal>(true,false);
        //svtStopSignal->setSendData(true);
        //svtStopSignal->setCanStop(false);
        EnqueueSignal(svtStopSignal);
    }
    else
    {
        if(IsWaitForGPSTicks(signal->GetTransactionId()))
        {
            bool isLast = IsLastByWaitForGPSTick(signal->GetTransactionId());
            SvtTrackPoint_t trackPoints;
            GenerateFakePos(&trackPoints);
            SendSvtData(trackPoints,isLast);//send fake position
            RemoveWaitForGPSTick(signal->GetTransactionId());
        }
        else 
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "svt transactionid error.\n");
        }
    }

    return true;
}

bool Svt::HandlePosition(std::shared_ptr<fsm::Signal> signal)
{
    State state = GetState();
    vpom::GNSSData gnss_position;
    SvtTrackPoint_t trackPoints;
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s processing,state:%d.\n", __FUNCTION__,static_cast<int>(state));
    
    if(signal->GetSignalType() != fsm::Signal::kGNSSPositionDataSignal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s received unexpected signal type:%d.\n", __FUNCTION__);
        return true;
    }

    std::shared_ptr<fsm::VpomGNSSPositionDataSignal> position_signal = std::static_pointer_cast<fsm::VpomGNSSPositionDataSignal>(signal);
    gnss_position = position_signal->GetPayload();
    
    if(gnss_position.datastatus == vpom::GNSS_DATA_INVALID)
    {
        RequestPosition(signal->GetTransactionId().GetSharedCopy());
    }
    else
    {
        bool isLast = IsLastByVpomRequestTick(signal->GetTransactionId());
        VpomToVds(gnss_position, &trackPoints);
        SendSvtData(trackPoints,isLast);
        RemoveVpomRequestTick(signal->GetTransactionId());
        
    }
    
    return true;
}
    
bool Svt::HandleVehicleComm(std::shared_ptr<fsm::Signal> signal)
{

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    bool result = false;
    
    SignalWithStatus_t signalStatus;
    result = GetSvtSignal(CHECKING,&signalStatus);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current has not checking signal.\n", __FUNCTION__);
        TriggerNextSvtSignal();
        return false;
    }

    switch (signal->GetSignalType())
    {
        case fsm::Signal::kCarMode:
        {
            result = HandleCarMode(signal);
            signalStatus.signal->setCheckedCarMode(true);
            signalStatus.signal->setCarMode(result);
            
            //test hardcode
            //signalStatus.signal->setCarMode(true);
            break;
        }
        case fsm::Signal::kStolenVehicleTracking:
        {
            result = HandleCarConfigSvt(signal);
            signalStatus.signal->setCheckedCarConfigSvt(true);
            signalStatus.signal->setCarConfigSvt(result);
            break;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                           "Svt::%s, received unexpected VehicleCommSignal.",__FUNCTION__);
    }

    //test hardcode
    signalStatus.signal->setCheckedCarConfigSvt(true);
    signalStatus.signal->setCarConfigSvt(true);
            
    //if no response from vc will be blocked,set timer?
    if(signalStatus.signal->getCheckedCarMode() && signalStatus.signal->getCheckedCarConfigSvt())
    {
        signalStatus.status = CHECKED;
        UpdateSvtSignal(signalStatus);
        
        TriggerNextSvtSignal();
    }
    else
    {
       return true;
    }

    if (signalStatus.signal->getCarMode() && signalStatus.signal->getCarConfigSvt())
    {
        State state = GetState();
        if(state == kNew || state == kStart)
        {
            RemoveMyTimeouts();
            RemoveAllWaitForGPSTicks();
    
            RunSvtSignal(signalStatus);
            
            SetState(kStart);
            std::shared_ptr<fsm::Signal> svtStartSignal = std::make_shared<SvtStartSignal>();
            EnqueueSignal(svtStartSignal);
        }
        else if(state == kPeriod)
        {
            RemoveMyTimeouts();
            RemoveAllWaitForGPSTicks();
            
            RunSvtSignal(signalStatus);
        
            SetState(kPeriod);
            std::shared_ptr<fsm::Signal> svtPeriodSignal = std::make_shared<SvtPeriodSignal>();
            EnqueueSignal(svtPeriodSignal);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "current state is NOK:%d,send incorrectState ack.\n", static_cast<int>(state));
            SendEndAck(signalStatus,incorrectState);
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "svt start condition is not correct,send conditionsNotCorrect ack.\n");
        SendEndAck(signalStatus,conditionsNotCorrect);
    }


    return true;
}

bool Svt::HandleCarMode(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);
    if(car_mode_response == 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car mode response error.\n", __FUNCTION__);
        return false;
    }

    vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();
    if (vc_return_value != vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car mode return value:%d.\n", __FUNCTION__,vc_return_value);
        return false;
    }

    vc::CarModeState car_mode_return = car_mode_response->GetData()->carmode;
    if (car_mode_return == vc::CAR_NORMAL || car_mode_return == vc::CAR_CRASH)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car mode OK", __FUNCTION__);
        return true;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car mode NOK", __FUNCTION__);
        return false;
    }
}

bool Svt::HandleCarConfigSvt(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    
    std::shared_ptr<fsm::StolenVehicleTrackingSignal> response =  std::static_pointer_cast<fsm::StolenVehicleTrackingSignal>(signal);
    if(response == 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car config svt response error.\n", __FUNCTION__);
        return false;
    }

    vc::ReturnValue vc_return_value = response->GetVcStatus();
    if (vc_return_value != vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car config svt return value:%d.\n", __FUNCTION__,vc_return_value);
        return false;
    }

    vc::StolenVehicleTracking svtConfig = response->GetData()->stolen_vehicle_tracking;
    if (svtConfig == vc::WITH_STOLEN_VEHICLE_TRACKING)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car config with svt.\n", __FUNCTION__);
        return true;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,car config without svt.\n", __FUNCTION__);
        return false;
    }
    
}

bool Svt::HandleAppsLcmResumeSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    return true;
}


bool Svt::RequestPosition(std::shared_ptr<fsm::TransactionId> tranId, bool isLast)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requesting position information.\n", __FUNCTION__);
    
    std::shared_ptr<fsm::VpomTransactionId> vpom_request_id = std::dynamic_pointer_cast<fsm::VpomTransactionId>(tranId);
    
    if(vpom_request_id == nullptr)
    {
        vpom_request_id = std::make_shared<fsm::VpomTransactionId>();
        
        std::shared_ptr<fsm::TimeoutTransactionId> waitForGPSTickId = WaitForGPSTime();
        if(waitForGPSTickId != nullptr)
        {
            RelationTranId_t relationId;
            relationId.requestId = *vpom_request_id;
            relationId.timeOutId = *waitForGPSTickId;
            relationId.isLast = isLast;
        
            AddWaitForGPSTick(relationId);
        }
    }
     
    bool ret_val = true;
    VpomIPositioningService& vpom_gnss_service_object = fsm::VpomSignalSource::GetInstance().GetPositioningServiceObject();
    MapTransactionId(vpom_request_id);
    ret_val = ret_val && vpom_gnss_service_object.GNSSPositionDataRawRequest(vpom_request_id->GetId());
    if(!ret_val)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "request position information fail.\n");
    }
    
    return ret_val;
}

std::shared_ptr<fsm::TimeoutTransactionId> Svt::WaitForGPSTime()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    bool result = false;

    SignalWithStatus_t runningSignal;
    result = GetSvtSignal(RUNNING,&runningSignal);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current has not running signal.\n", __FUNCTION__);
        TriggerNextSvtSignal();
        return nullptr;
    }
    
    std::shared_ptr<SvtRequest_t> request = runningSignal.signal->GetRequestPayload();
    long int interval = request->interval;

    if(interval > 0 && m_waitForGPSTime > static_cast<unsigned int>(interval))
        m_waitForGPSTime = static_cast<unsigned int>(interval);

    std::lock_guard<std::mutex> lock(m_request_timeout_mutex);
    
    fsm::TimeoutTransactionId waitForGPSTickId;
    MapTransactionId(waitForGPSTickId.GetSharedCopy());
    RequestTimeout(waitForGPSTickId, static_cast<std::chrono::seconds>(m_waitForGPSTime), false);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Timer waitForGPSTime timeOut %lds ,waitForGPSTickId:%lld\n", m_waitForGPSTime,waitForGPSTickId.GetId());

    return std::make_shared<fsm::TimeoutTransactionId>(waitForGPSTickId);
}


bool Svt::SendSvtData(SvtTrackPoint_t trackPoints,bool isLast)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s send svt data to tsp.\n", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "position.latitude:%ld.\n", trackPoints.latitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "position.longitude:%ld.\n", trackPoints.longitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "position.altitude:%ld.\n", trackPoints.altitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "posCanBeTrusted:%d.\n", trackPoints.posCanBeTrusted);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "carLocatorStatUploadEn:%d.\n", trackPoints.carLocatorStatUploadEn);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "marsCoordinates:%d.\n", trackPoints.marsCoordinates);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "isSVT:%d.\n", trackPoints.isSVT);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "speed:%ld.\n", trackPoints.speed);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "systemTime.seconds:%ld.\n", trackPoints.systemTimeSeconds);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "systemTime.milliseconds:%ld.\n", trackPoints.systemTimeMilliseconds);

    bool result = false;
    SignalWithStatus_t runningSignal;
    result = GetSvtSignal(RUNNING,&runningSignal);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,current has not running signal.\n", __FUNCTION__);
        TriggerNextSvtSignal();
        return false;
    }

    runningSignal.signal->SetTrackPointPayload(trackPoints);

    fsm::VocmoSignalSource vocmo_signal_source;
    result = vocmo_signal_source.GeelySendMessage(runningSignal.signal, true);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Svt message sent ok.\n");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Svt message send fail.\n");
    }

    if(isLast)
    {
        std::shared_ptr<SvtStopSignal> svtStopSignal = std::make_shared<SvtStopSignal>(false,true);
        //svtStopSignal->setSendData(false);
        //svtStopSignal->setCanStop(true);
        EnqueueSignal(svtStopSignal);
    }
    
    Timestamp::TimeVal utcTime = GetUtcTime();
    long int endIntervalTmp = m_execteTime - (utcTime/1000000);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, endIntervalTmp: %d, GLY_APP_SVT_PREDEFINED_TIMER: %d", __FUNCTION__, endIntervalTmp, GLY_APP_SVT_PREDEFINED_TIMER);
    if (GLY_APP_SVT_PREDEFINED_TIMER < endIntervalTmp){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, m_SvtLcmTransactionId_ adds: 0x%x, GLY_APP_SVT_PREDEFINED_TIMER(%d) <= remain_time(%d)",
                            __FUNCTION__, m_SvtLcmTransactionId_.get(), GLY_APP_SVT_PREDEFINED_TIMER, endIntervalTmp);
        if ( !AppsLcm::GetInstance().pm_request_feature(m_SvtLcmTransactionId_, false) ){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, operate pm_request_feature(false) failed\n", __FUNCTION__);
        }
    } else {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s, power keepawake. GLY_APP_SVT_PREDEFINED_TIMER(%d) > remain_time(%d)\n", __FUNCTION__, GLY_APP_SVT_PREDEFINED_TIMER, endIntervalTmp);
    }
    
    return result;
}

bool Svt::SendEndAck(SignalWithStatus_t signalStatus,long errorCode)
{
    SvtServiceResult_t serviceResult;
    if(errorCode == -1)
    {
        serviceResult.operationSucceeded = true;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,operationSucceeded:%d\n", 
            __FUNCTION__,serviceResult.operationSucceeded);
    }
    else
    {
        serviceResult.operationSucceeded = false;
        serviceResult.errorCode = errorCode;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s,operationSucceeded:%d,errorCode:%ld\n", 
            __FUNCTION__,serviceResult.operationSucceeded,serviceResult.errorCode);
    }
    
    signalStatus.signal->SetResultPayload(serviceResult);
    
    fsm::VocmoSignalSource vocmo_signal_source;
    bool result = vocmo_signal_source.GeelySendMessage(signalStatus.signal, true);
    if (result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Svt ack sent ok.\n");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Svt ack send fail.\n");
    }

    if(errorCode != -1)
    {
        signalStatus.status = COMPLETE;
        UpdateSvtSignal(signalStatus);
        
        TriggerNextSvtSignal();
    }

    
    return result;
}

bool Svt::VpomToVds(vpom::GNSSData vpomPosition, SvtTrackPoint_t *vdsTrackPoint)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    
    if (vdsTrackPoint == nullptr) {
        return false;
    }

    vdsTrackPoint->latitude = vpomPosition.position.longlat.latitude; // Latitude
    vdsTrackPoint->longitude = vpomPosition.position.longlat.longitude; // Longitude
    vdsTrackPoint->altitude = vpomPosition.position.altitude; // Altitide from mean sea level
    vdsTrackPoint->posCanBeTrusted = true;//TODO
    vdsTrackPoint->carLocatorStatUploadEn = true;
    vdsTrackPoint->marsCoordinates = false;
    vdsTrackPoint->isSVT = true;
    vdsTrackPoint->speed = vpomPosition.movement.speed;

    Timestamp time;
    std::tm thresholdTime;
    thresholdTime.tm_year = (vpomPosition.utcTime.year - 1900); // UTC year
    thresholdTime.tm_mon = vpomPosition.utcTime.month; // UTC month
    thresholdTime.tm_mday = vpomPosition.utcTime.day; // UTC day
    thresholdTime.tm_hour = vpomPosition.utcTime.hour; // UTC hour
    thresholdTime.tm_min = vpomPosition.utcTime.minute; // UTC minute
    thresholdTime.tm_sec = vpomPosition.utcTime.second; // UTC second
    thresholdTime.tm_isdst = 0;
    std::time_t t = std::mktime(&thresholdTime);
    time.fromEpochTime(t);

    vdsTrackPoint->systemTimeSeconds = time.epochMicroseconds()/1000000;
    vdsTrackPoint->systemTimeMilliseconds= (time.epochMicroseconds()%1000000)/1000;
    
    return true;
}

bool Svt::GenerateFakePos(SvtTrackPoint_t *vdsTrackPoint)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    
    if (vdsTrackPoint == nullptr) {
        return false;
    }

    vdsTrackPoint->isSVT = true;
    vdsTrackPoint->systemTimeSeconds = 0;
    vdsTrackPoint->systemTimeMilliseconds= 0;
    vdsTrackPoint->speed = 0;
    vdsTrackPoint->latitude = 0; // Latitude
    vdsTrackPoint->longitude = 0; // Longitude
    vdsTrackPoint->altitude = 0; // Altitide from mean sea level
    vdsTrackPoint->posCanBeTrusted = true;//TODO
    vdsTrackPoint->carLocatorStatUploadEn = true;
    vdsTrackPoint->marsCoordinates = false;

    return true;
}

bool Svt::CheckStartCondition(SignalWithStatus_t signalStatus)
{
    bool result = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    State state = GetState();

    result = CheckSvtParam(signalStatus.signal->GetRequestPayload());
    if(!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "svt param is invalid,send invalidData ack.\n");
        SendEndAck(signalStatus,invalidData);
        return result;
    }

    if(state == kStop || state == kDone)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "current state is NOK:%d,send incorrectState ack.\n", static_cast<int>(state));
        SendEndAck(signalStatus,incorrectState);
        return false;
    }

    result = CheckProvision();
    if(!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "svt is not provisioned,send conditionsNotCorrect ack.\n");
        SendEndAck(signalStatus,conditionsNotCorrect);
        return result;
    }

    result = RequestCarMode();
    if(!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "request car mode fail,send conditionsNotCorrect ack.\n");
        SendEndAck(signalStatus,conditionsNotCorrect);
        return result;
    }
    
    //result = RequestCarConfigSvt();
    if(!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "request car config svt fail,send conditionsNotCorrect ack.\n");
        SendEndAck(signalStatus,conditionsNotCorrect);
        return result;
    }

    
    return result;
    
}

bool Svt::CheckStopCondition(SignalWithStatus_t signalStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    State state = GetState();
    std::shared_ptr<SvtRequest_t> request = signalStatus.signal->GetRequestPayload();

    bool result = HasInProcessSvtSignal(signalStatus);
    if(!result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "has no in processing signal,send conditionsNotCorrect ack.\n");
        SendEndAck(signalStatus,conditionsNotCorrect);
        return false;
    }
    
    if(state == kStop || state == kDone)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "current state is NOK:%d,send incorrectState ack.\n", static_cast<int>(state));
        SendEndAck(signalStatus,incorrectState);
        return false;
    }
    
    RemoveMyTimeouts();
    RemoveAllWaitForGPSTicks();
    
    RunSvtSignal(signalStatus);
    
    SetState(kStop);
    std::shared_ptr<SvtStopSignal> svtStopSignal = std::make_shared<SvtStopSignal>(true,false);
    //svtStopSignal->setSendData(true);
    //svtStopSignal->setCanStop(false);
    EnqueueSignal(svtStopSignal);
        
    return true;

}
bool Svt::CheckProvision()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CheckProvision, provision OK.\n");
    
    return true;
}

bool Svt::RequestCarMode()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requesting modes\n", __FUNCTION__);

    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
    
    if (igen.Request_CarMode(vehicle_comm_request_id->GetId()) == vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Svt::%s requests for car mode sent to VehicleComm.",__FUNCTION__);
        return true;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Svt::%s failed to issue request to VehicleComm.",__FUNCTION__);
        return false;
    }

}

bool Svt::RequestCarConfigSvt()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requesting svt car config\n", __FUNCTION__);

    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id = std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
    
    if (igen.Request_GetCarCfgStolenVehicleTracking(vehicle_comm_request_id->GetId()) == vc::RET_OK)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Svt::%s requests for car config svt sent to VehicleComm.",__FUNCTION__);
        return true;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "Svt::%s failed to issue request to VehicleComm.",__FUNCTION__);
        return false;
    }

}

bool Svt::CheckSvtParam(std::shared_ptr<SvtRequest_t> request)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    
    Timestamp::TimeVal utcTime = GetUtcTime();

    long long int lCurrUtcTime = utcTime/1000;
    long long int lStartTime = VdsTimestampToMilliSec(request->scheduledTimeSeconds,request->scheduledTimeMilliseconds);
    long long int lEndTime = VdsTimestampToMilliSec(request->endTimeSeconds,request->endTimeMilliseconds);
    
    std::string strCurrUtcTime = UtcTimeValToString(lCurrUtcTime*1000);
    std::string strStartTime = UtcTimeValToString(lStartTime*1000);
    std::string strEndTime = UtcTimeValToString(lEndTime*1000);
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "current time is :%lld,svt start time is:%lld,stop time is:%lld,svt interval is:%ds.\n", 
        lCurrUtcTime,lStartTime,lEndTime,request->interval);
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "current time is :%s,svt start time is:%s,stop time is:%s,svt interval is:%ds.\n", 
        strCurrUtcTime.c_str(),strStartTime.c_str(),strEndTime.c_str(),request->interval);
    
    if(lStartTime >= lEndTime)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "svt start time is:%lld,stop time is:%lld, start time must be earlier than stop time.\n", lStartTime, lEndTime);
        return false;
    }
    
    if(lEndTime <= lCurrUtcTime)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "svt stop time is:%lld,current time is:%lld, stop time must be later than current time.\n", lEndTime, lCurrUtcTime);
        return false;
    }

    unsigned int svtMinIntervalTime;
    bool result = m_vocpersist.GetData(vocpersistdatamgr::VOC_PERSISTENT_ATTRIBUTE::SVT_MIN_TRACKING_INTERVAL, svtMinIntervalTime);
    
    if(result)
    {
        if(request->interval < static_cast<long>(svtMinIntervalTime))
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "svt interval is:%ld,svt min interval is:%d, svt interval must be bigger than min interval.\n", request->interval, svtMinIntervalTime);
            return false;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "get svt min interval time persistent data failed!\n");
        return false;
    }

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "CheckSvtParam, check svt trigger param OK.\n");
    return true;
}

void Svt::TriggerNextSvtSignal()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);

    RemoveCompleteSvtSignal();
    if(!HasInProcessSvtSignal())
    {
        SetState(kDone);
    }
    else if(CanHandleNextSvtSignal())
    {
        SignalWithStatus_t signalStatus;
        bool result = GetSvtSignal(UNCHECK,&signalStatus);
        if(result)
        {
            signalStatus.status = CHECKING;
            UpdateSvtSignal(signalStatus);
            HandleSvtRequest(signalStatus);
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s not exist uncheck status signal.\n", __FUNCTION__);
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s already exist checking status signal.\n", __FUNCTION__);
    }
}

std::string Svt::DateTimeToString(Timestamp ts)
{
    char buff[128];
    std::string microsec;
    
    std::time_t t = ts.epochTime();
    std::tm *tm = std::localtime(&t);
    
    if(tm!=0)
    {
        std::strftime(buff,128, "%Y-%m-%d %H:%M:%S", tm);
        
        microsec = std::to_string(ts.epochMicroseconds() % 1000000);

        if (microsec.length() < 6) {
            microsec = std::string(6 - microsec.length(), '0') + microsec;
        }
    }
    
    return std::string(buff) + "." + microsec;
}

std::string Svt::UtcTimeValToString(long long int utcVal)
{
    Timestamp ts(utcVal);

    return DateTimeToString(ts);
}

long long int Svt::VdsTimestampToMilliSec(long long int seconds,long long int milliseconds)
{
    long long int totalMilliSec = (seconds * 1000) + milliseconds;

    return totalMilliSec;
}


void Svt::AddWaitForGPSTick(RelationTranId_t relationId)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requestId:%lld,timeOutId:%lld\n"
        , __FUNCTION__
        ,relationId.requestId.GetId()
        ,relationId.timeOutId.GetId());
    
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    m_relationIds.push_back(relationId);
}

void Svt::RemoveWaitForGPSTick(const fsm::TransactionId& tickId)
{
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    for (auto it = m_relationIds.begin(); it != m_relationIds.end(); ) 
    {
        if(tickId == it->timeOutId)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requestId:%lld,timeOutId:%lld\n"
                , __FUNCTION__
                ,it->requestId.GetId()
                ,it->timeOutId.GetId());
            
            RemoveTimeout(it->timeOutId);
            it = m_relationIds.erase(it);
        }
        else 
        {
            ++it;
        }
    }
}

void Svt::RemoveVpomRequestTick(const fsm::TransactionId& tickId)
{
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    for (auto it = m_relationIds.begin(); it != m_relationIds.end(); ) 
    {
        if(tickId == it->requestId)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requestId:%lld,timeOutId:%lld\n"
                , __FUNCTION__
                ,it->requestId.GetId()
                ,it->timeOutId.GetId());
            
            RemoveTimeout(it->timeOutId);
            it = m_relationIds.erase(it);
        }
        else 
        {
            ++it;
        }
    }
}

bool Svt::IsLastByWaitForGPSTick(const fsm::TransactionId& tickId)
{
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    
    for (auto it = m_relationIds.begin(); it != m_relationIds.end(); ) 
	{
        if (tickId == it->timeOutId)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requestId:%lld,timeOutId:%lld,isLast:%d\n"
                , __FUNCTION__
                ,it->requestId.GetId()
                ,it->timeOutId.GetId()
                ,it->isLast);
            
			return it->isLast;
        }
    }
    
	return false;
	
}

bool Svt::IsLastByVpomRequestTick(const fsm::TransactionId& tickId)
{
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    
    for (auto it = m_relationIds.begin(); it != m_relationIds.end(); ) 
	{
        if (tickId == it->requestId)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requestId:%lld,timeOutId:%lld,isLast:%d\n"
                , __FUNCTION__
                ,it->requestId.GetId()
                ,it->timeOutId.GetId()
                ,it->isLast);
            
			return it->isLast;
        }
    }
    
	return false;
	
}


bool Svt::IsWaitForGPSTicks(const fsm::TransactionId& tickId)
{
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    
    for (auto it = m_relationIds.begin(); it != m_relationIds.end(); ) 
	{
        if (tickId == it->timeOutId)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s requestId:%lld,timeOutId:%lld\n"
                , __FUNCTION__
                ,it->requestId.GetId()
                ,it->timeOutId.GetId());
            
			return true;
        }
    }
    
	return false;
	
}

void Svt::RemoveAllWaitForGPSTicks()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_relationIds_mutex);
    m_relationIds.clear();
}

void Svt::EnqueueSvtSignal(SignalWithStatus_t signalStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s \n", __FUNCTION__ );
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s signal addr:%0x\n", __FUNCTION__,signalStatus.signal.get());
    
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);
    m_svtSignalStatusList.push_back(signalStatus);
}

void Svt::UpdateSvtSignal(SignalWithStatus_t signalStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s status:%d signal addr:%0x\n"
        , __FUNCTION__,signalStatus.status,signalStatus.signal.get());
    
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);
    
    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->signal == signalStatus.signal)
        {
            it->status = signalStatus.status;
            return;
        }
        else
        {
            ++it;
        }
        
    }
}

void Svt::RunSvtSignal(SignalWithStatus_t signalStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s \n", __FUNCTION__ );
    
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);
    
    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->signal != signalStatus.signal)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s complete status:%d signal addr:%0x\n", __FUNCTION__,it->status,it->signal.get());
            if(it->status == RUNNING)
                it->status = COMPLETE;
        }
        else
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s running status:%d signal addr:%0x\n", __FUNCTION__,it->status,it->signal.get());
            it->status = RUNNING;
        }

        ++it;
    }
}

bool Svt::GetSvtSignal(SignalStatus status, SignalWithStatus_t* signalStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);

    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->status == status)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s status:%d signal addr:%0x\n", __FUNCTION__,it->status,it->signal.get());
            signalStatus->status = it->status;
            signalStatus->signal = it->signal;
            return true;
        }
        
        ++it;
    }

    return false;
}

bool Svt::CanHandleNextSvtSignal()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);

    if(m_svtSignalStatusList.size() == 0)
        return false;

    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->status == CHECKING)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s status:CHECKING signal addr:%0x\n", __FUNCTION__,it->signal.get());
            return false;
        }
        
        ++it;
    }

    return true;
}

bool Svt::HasInProcessSvtSignal()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);

    if(m_svtSignalStatusList.size() == 0)
        return false;
    
    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->status != COMPLETE)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s status:%d signal addr:%0x\n", __FUNCTION__,it->status,it->signal.get());
            return true;
        }
        
        ++it;
    }

    return false;
}

bool Svt::HasInProcessSvtSignal(SignalWithStatus_t signalStatus)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);

    if(m_svtSignalStatusList.size() == 0)
        return false;
    
    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->status != COMPLETE 
            && it->signal != signalStatus.signal)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s status:%d queue signal addr:%0x param[in] signal addr:%0x\n"
                , __FUNCTION__,it->status,it->signal.get(),signalStatus.signal.get());
            return true;
        }
        
        ++it;
    }

    return false;
}


void Svt::RemoveCompleteSvtSignal()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s\n", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_signal_status_mutex);
    
    for (auto it = m_svtSignalStatusList.begin(); it != m_svtSignalStatusList.end();) 
	{
        if (it->status == COMPLETE)
        {   
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "Svt::%s signal addr:%0x\n", __FUNCTION__,it->signal.get());
            it = m_svtSignalStatusList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
