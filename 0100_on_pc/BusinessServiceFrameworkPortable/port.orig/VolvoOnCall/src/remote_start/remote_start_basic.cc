///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_basic.cpp
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   12-Nov-2018
///////////////////////////////////////////////////////////////////

#include "remote_start/remote_start_basic.h"

#include "dlt/dlt.h"
DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call {
    RMTStartBasic::RMTStartBasic():
        fsm::SmartTransaction(kNew),
        m_igen(fsm::VehicleCommSignalSource::GetInstance().GetVehicleCommClientObject().GetGeneralInterface()),
        m_cmd(CMD_NoReq),
        m_vocpersist(vocpersistdatamgr::VocPersistDataMgr::GetInstance()),
        m_VFCTimeout(-1), m_TSPTimeout(-1), m_RequestTimeout(-1),
        m_flags({false, false, false, false}),
        m_ErsStartApplSts(vc::ErsStrtApplSts_ErsStsOff),
        m_EngStlWdSts(vc::EngSt1_RunngRemStrtd)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s constructor start", __FUNCTION__);
    }

    void RMTStartBasic::BasicInit(SendSingalCallBack SendSignal_cb)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s", __FUNCTION__);
        InitFlagsStruct();
        m_callback = SendSignal_cb;
        SignalFunction Handle_Reuqest_Signal =
            std::bind(&RMTStartBasic::Handle_RequestSignal, this, std::placeholders::_1);
        SignalFunction Handle_CarMode_Signal =
            std::bind(&RMTStartBasic::Handle_CarModeSignal, this, std::placeholders::_1);
        SignalFunction Handle_UsageMode_Signal =
            std::bind(&RMTStartBasic::Handle_UsageModeSignal, this, std::placeholders::_1);
        SignalFunction Handle_EngineLevel_Signal =
            std::bind(&RMTStartBasic::Handle_EngineLevelSignal, this, std::placeholders::_1);
        SignalFunction Handle_EngineStates_Signal =
            std::bind(&RMTStartBasic::Handle_EngineStatesSignal, this, std::placeholders::_1);
        SignalFunction Handle_StartEngine_Signal =
            std::bind(&RMTStartBasic::Handle_StartEngineSignal, this, std::placeholders::_1);
        SignalFunction Handle_DelayEngine_Signal =
            std::bind(&RMTStartBasic::Handle_DelayEngineSignal, this, std::placeholders::_1);
        SignalFunction Handle_Response_Signal =
            std::bind(&RMTStartBasic::Handle_ResponseSignal, this, std::placeholders::_1);
        SignalFunction Handle_Timeout_Signal =
            std::bind(&RMTStartBasic::Handle_GlyTimeout, this, std::placeholders::_1);

        StateMap state_map = {
            {kNew,                      {nullptr,
                                            {{fsm::kVDServiceRce, Handle_Reuqest_Signal},
                                            {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                            {kCheckPreCondition, kDone}}},
            {kCheckPreCondition,        {nullptr,
                                            {{fsm::Signal::kCarMode, Handle_CarMode_Signal},
                                            {fsm::Signal::kCarUsageMode, Handle_UsageMode_Signal},
                                            {fsm::Signal::kRMTEngineSignal, Handle_EngineStates_Signal},
                                            {fsm::Signal::kElectEngLvlSignal, Handle_EngineLevel_Signal},
                                            {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                            {kOperateEgnine, kHandleRequest, kDone}}},
            {kOperateEgnine,            {nullptr,
                                            {
                                            {VocInternalSignalTypes::kAppStartEngineResponseSignal, Handle_StartEngine_Signal}, //start engine signal
                                            {fsm::Signal::kDelayEngineRinningtimeSignal, Handle_DelayEngine_Signal}, //delay engine signal
                                            {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                            {kHandleRequest, kDone}}},
            {kHandleRequest,            {nullptr,
                                            {{fsm::Signal::kTimeout, Handle_Timeout_Signal},
                                            {fsm::Signal::kRemoteCliamteSignal, Handle_Response_Signal},
                                            {fsm::Signal::kRemoteSeatHeatSignal, Handle_Response_Signal},
                                            {fsm::Signal::kRemoteSeatVentiSignal, Handle_Response_Signal},
                                            {fsm::Signal::kRemoteSteerWhlHeatSignal, Handle_Response_Signal}}, //handle response signal
                                            {kWaitingStop, kHandleRequest, kDone}}},
            {kWaitingStop,              {nullptr,
                                            {{fsm::Signal::kTimeout, Handle_Timeout_Signal}}, //handle runng timer timeout signal
                                            {kTimeoutStop, kDone}}},
            {kDone,                     {nullptr,
                                            {{fsm::kVDServiceRce, Handle_Reuqest_Signal},
                                            {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                            {kCheckPreCondition}}},
            {kTimeoutStop,              {nullptr,
                                            {{fsm::kVDServiceRce, Handle_Reuqest_Signal},
                                            {fsm::Signal::kRMTEngineSignal, Handle_EngineStates_Signal}, //handle engine states signal
                                            {fsm::Signal::kTimeout, Handle_Timeout_Signal}},
                                            {kCheckPreCondition, kHandleRequest, kDone}}},
        };
        SetStateMap(state_map);
        MapSignalType(VocInternalSignalTypes::kAppStartEngineResponseSignal);
        MapSignalType(fsm::Signal::kCarMode);
        MapSignalType(fsm::Signal::kCarUsageMode);
        MapSignalType(fsm::Signal::kRMTEngineSignal);
        MapSignalType(VocInternalSignalTypes::kAppStartEngineResponseSignal);
        MapSignalType(fsm::Signal::kDelayEngineRinningtimeSignal);
        MapSignalType(fsm::Signal::kRemoteCliamteSignal);
        MapSignalType(fsm::Signal::kRemoteSeatHeatSignal);
        MapSignalType(fsm::Signal::kRemoteSeatVentiSignal);
        MapSignalType(fsm::Signal::kRemoteSteerWhlHeatSignal);
        MapSignalType(fsm::Signal::kTimeout);

        if (!m_vocpersist.GetData(vocpersistdatamgr::RCE_RPC_VFC_TIMEOUT, m_VFCTimeout)){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "~RMTStartBasic::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
            m_VFCTimeout = GLY_CLIMA_RPC_VFC_TIMEOUT_DEFAULT;
        } else {
            if ((GLY_CLIMA_RPC_VFC_TIMEOUT_MIX > m_VFCTimeout) ||
                (GLY_CLIMA_RPC_VFC_TIMEOUT_MAX < m_VFCTimeout)){
                    m_VFCTimeout = GLY_CLIMA_RPC_VFC_TIMEOUT_DEFAULT;
                }
        }
        if (!m_vocpersist.GetData(vocpersistdatamgr::RCE_CLIMA_REQ_TIMEOUT, m_RequestTimeout)){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "~RMTStartBasic::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
            m_RequestTimeout = GLY_CLIMA_REQUEST_TIME_DEFAULT;
        } else {
            if ((GLY_CLIMA_REQUEST_TIME_MIN > m_RequestTimeout) ||
                (GLY_CLIMA_REQUEST_TIME_MAX < m_RequestTimeout)){
                    m_RequestTimeout = GLY_CLIMA_REQUEST_TIME_DEFAULT;
                }
        }
        if (!m_vocpersist.GetData(vocpersistdatamgr::RCE_TSP_RESPONSE_TIME, m_TSPTimeout)){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "~RMTStartBasic::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
            m_TSPTimeout = GLY_CT_TSP_SET_RESPONSE_TIME_DEFAULT;
        } else {
            if ((GLY_CT_TSP_SET_RESPONSE_TIME_MIN > m_TSPTimeout) ||
                (GLY_CT_TSP_SET_RESPONSE_TIME_MAX < m_TSPTimeout)){
                    m_TSPTimeout = GLY_CT_TSP_SET_RESPONSE_TIME_DEFAULT;
                }
        }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, m_VFCTimeout: %d, end.", __FUNCTION__, m_VFCTimeout);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, m_RequestTimeout: %d, end.", __FUNCTION__, m_RequestTimeout);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, m_TSPTimeout: %d, end.", __FUNCTION__, m_TSPTimeout);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s end", __FUNCTION__);
    }

    RMTStartBasic::~RMTStartBasic(){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::~%s, destroy", __FUNCTION__)
    }

    std::shared_ptr<volvo_on_call::GlyVdsRceSignal> RMTStartBasic::CreateGlyVdsRceSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s constructor start", __FUNCTION__);
        return std::dynamic_pointer_cast<volvo_on_call::GlyVdsRceSignal>(signal);
    }

    bool RMTStartBasic::Request_PreCondition(ServiceType service_type)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s", __FUNCTION__);
        bool ret = false;

        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        SetState(kCheckPreCondition);
        InitFlagsStruct();
        if ((m_igen.Request_CarMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
            (m_igen.Request_CarUsageMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
            (m_igen.Request_GetEngineStates( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
            (m_igen.Request_ElectEnergyLevel( vehicle_comm_request_id->GetId()) == vc::RET_OK))
        {
            ret = true;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                    "RMTStartBasic::%s, SUCCESS, state(1): %d", __FUNCTION__, GetState());
        } else {
            ret = false;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                    "RMTStartBasic::%s, Failed", __FUNCTION__);
            HandleFailed(kExecutionFailure);
        }
        return ret;
    }

    bool RMTStartBasic::Request_VFCActive(bool keep_wake)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);

        vc::ReqVFCActivate request;
        request.id = GLY_TCAM_REMOTE_START_ACTIVE_VFC_ID;
        request.type = keep_wake;
        //SetState(kWaitVFCResponse);
        if (m_igen.Request_VFCActivate(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK) 
        {
            //TODO
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                "RMTStartBasic::%s, SUCCESS", __FUNCTION__);
        } else {
            ret = false;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                "RMTStartBasic::%s, Failed", __FUNCTION__);
            HandleFailed(kExecutionFailure);
        }
        return ret;
    }

    bool RMTStartBasic::Request_StartEngine()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        std::shared_ptr<volvo_on_call::AppEngineStatRequestSignal> signal =
                                    std::make_shared<volvo_on_call::AppEngineStatRequestSignal>();
        std::shared_ptr<RMT_ClimatizationRequest_t> _req = m_pGlyVdsRceSignal->GetRequestPayload();
        if (nullptr == _req.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. nullptr == _req", __FUNCTION__);
            return false;
        }
        signal->SetValue(_req->m_duration_time, vc::ErsCmd_ErsCmdOn);
        m_callback(signal);
        return true;
    }

    bool RMTStartBasic::Request_DelayEnginneRunningTimer()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        SetState(kOperateEgnine);
        vc::Req_DelayEngineRunngTime req;
        std::shared_ptr<RMT_ClimatizationRequest_t> _req = m_pGlyVdsRceSignal->GetRequestPayload();
        if (nullptr == _req.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. nullptr == _req", __FUNCTION__);
            return false;
        }
        req.telmEngDelayTi = _req->m_duration_time;
        if ( vc::RET_OK == m_igen.Request_DelayEngineRunningTime(&req, vehicle_comm_request_id->GetId()))
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, SUCCESS, state(2): %d", __FUNCTION__, GetState());
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, FAILED", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        //TODO
        return ret;
    }

    /**
     * @brief:
     *          handle steps:
     *              1. check PreCondition
     *              2. Delay engine running time or start engine
     *              3. Handle Request which comes from TSP
     *              4. Handle Response after sent request to Vuc
     *              5. Reponse to TSP, Handle Request successful or failed
    **/
    bool RMTStartBasic::Handle_RequestSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        //add timeout
        m_TSPTimeoutId = RequestTimeout(std::chrono::seconds(m_TSPTimeout), false);
        
        m_pGlyVdsRceSignal.reset();
        m_pGlyVdsRceSignal = std::dynamic_pointer_cast<volvo_on_call::GlyVdsRceSignal>(signal);
        Request_PreCondition(m_pGlyVdsRceSignal->GetServiceType());
        //SendRequest(m_pGlyVdsRceSignal->GetClimateRequest());
        return true;
    }

    bool RMTStartBasic::Handle_UsageModeSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        //TODO
        bool ret = false;
        std::shared_ptr<fsm::CarUsageModeSignal> car_usage_mode_response = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);
        vc::ReturnValue vc_return_value = car_usage_mode_response->GetVcStatus();
        if (vc::RET_OK != vc_return_value){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMTStartBasic::%s, Failed vc_return_value != vc::RET_OK.", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        vc::CarUsageModeState car_usage_mode_return = car_usage_mode_response->GetData()->usagemode;
        if (kCheckPreCondition == GetState())
        {
            
            if (CMD_Stop == m_pGlyVdsRceSignal->GetOperateCommand()){ 
                if (vc::CAR_INACTIVE == car_usage_mode_return)
                {
                    m_flags.m_UsageModeSignalFlage = true;
                    ret = true;
                    OperateEingne();
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, usage is not in  inactive status(CMD_Stop).", __FUNCTION__);
                    HandleFailed(kConditionsNotCorrect);
                }
            } else if(CMD_Start == m_pGlyVdsRceSignal->GetOperateCommand()){
                if ((vc::CAR_ABANDONED == car_usage_mode_return) ||
                    (vc::CAR_INACTIVE == car_usage_mode_return))
                {
                    m_flags.m_UsageModeSignalFlage = true;
                    ret = true;
                    OperateEingne();
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, usage is not in abandoned or inactive status(CMD_Start).", __FUNCTION__);
                    HandleFailed(kConditionsNotCorrect);
                }
            }
        }
        return ret;
    }

    bool RMTStartBasic::Handle_CarModeSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);
        vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "RMTStartBasic::%s, Get CarMode Failed.", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        vc::CarModeState car_mode_return = car_mode_response->GetData()->carmode;
        if (kCheckPreCondition == GetState())
        {
            if (car_mode_return == vc::CAR_NORMAL)
            {
                m_flags.m_CarModeSignalFlags = true;
                ret = true;
                OperateEingne();
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, car Mode is not in Normal status.", __FUNCTION__);
                HandleFailed(kConditionsNotCorrect);
            }
        }
        return ret;
    }

    bool RMTStartBasic::Handle_EngineLevelSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        bool ret = false;
        if (CMD_Stop == m_pGlyVdsRceSignal->GetOperateCommand()){ //stop climatization ignore this signal;
            return true;
        }
        std::shared_ptr<fsm::ElectEngLvlSignal> m_engine_level =  std::static_pointer_cast<fsm::ElectEngLvlSignal>(signal);
        vc::ReturnValue vc_return_value = m_engine_level->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "RMTStartBasic::%s, Get engine Level Failed.", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        if (kCheckPreCondition == GetState()){
            if (vc::ENERGY_LVL_NORMAL == m_engine_level->GetData()->level)
            {
                m_flags.m_EngineLevelFlags = true;
                ret = true;
                OperateEingne();
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, Engine Level is not vc::ENERGY_LVL_NORMAL.", __FUNCTION__);
                HandleFailed(kConditionsNotCorrect);
            }
        }
        return ret;
    }

    bool RMTStartBasic::Handle_EngineStatesSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::RMTEngineSignal> _pRMTEngineSignal_ =  std::static_pointer_cast<fsm::RMTEngineSignal>(signal);
        vc::ReturnValue vc_return_value = _pRMTEngineSignal_->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "RMTStartBasic::%s, usage is not in abandoned or inactive status.", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, ErsStrtRes: %d ",
                    __FUNCTION__, _pRMTEngineSignal_->GetData()->ersStrtRes);
        //DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s,, EngStlWdSts: %d ",
                    //__FUNCTION__, _pRMTEngineSignal_->GetData()->engSt1WdSts);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, ErsStrtApplSts: %d ",
                    __FUNCTION__, _pRMTEngineSignal_->GetData()->ersStrtApplSts);
        m_ErsStartApplSts = _pRMTEngineSignal_->GetData()->ersStrtApplSts;  //get engine status(running or off or starting)
        //m_EngStlWdSts = _pRMTEngineSignal_->GetData()->engSt1WdSts;
        switch (GetState()){
            case kCheckPreCondition:{
                if((vc::EngSt1_RunngRemStrtd == vc::EngSt1_RunngRemStrtd) &&
                (vc::ErsStrtApplSts_ErsStsRunng == _pRMTEngineSignal_->GetData()->ersStrtApplSts))
                {
                    m_flags.m_EngineStatesFlags = true;
                    ret = true;
                    OperateEingne();
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                                "RMTStartBasic::%s, Engine status is error.", __FUNCTION__);
                    HandleFailed(kConditionsNotCorrect);
                }
                break;
            }
            case kTimeoutStop:{ // <*Auto to stop
                if(vc::ErsStrtApplSts_ErsStsOff != m_ErsStartApplSts){
                    //TODO stop remote climatization
                    std::shared_ptr<RMT_ClimatizationRequest_t> req = std::make_shared<RMT_ClimatizationRequest_t>();
                    if (nullptr == req.get()){
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, nullptr == req", __FUNCTION__);
                        return false;
                    }
                    req->m_operateCmd = CMD_TimeoutStop;
                    //SetState(kHandleRequest);
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, state: %d.", __FUNCTION__, GetState());
                    SendRequest(req);
                    ret = true;
                } else {
                    SetState(kDone);
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                                "RMTStartBasic::%s, Engine has been stopped, ignore this timeout signal", __FUNCTION__);
                }
                break;
            }
            default:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                                "RMTStartBasic::%s, Unknow states to handle engine states signal.", __FUNCTION__);
                break;
            }
        }

        if(kCheckPreCondition == GetState()){
            if((vc::EngSt1_RunngRemStrtd == vc::EngSt1_RunngRemStrtd) &&
                (vc::ErsStrtApplSts_ErsStsRunng == _pRMTEngineSignal_->GetData()->ersStrtApplSts))
            {
                m_flags.m_EngineStatesFlags = true;
                ret = true;
                OperateEingne();
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, Engine status is error.", __FUNCTION__);
                HandleFailed(kConditionsNotCorrect);
            }
        }
        return ret;
    }

    bool RMTStartBasic::Handle_VFCSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s", __FUNCTION__);
        std::shared_ptr<fsm::VFCActivateSignal> _pVFCActivateSignal = std::static_pointer_cast<fsm::VFCActivateSignal>(signal);
        vc::ReturnValue vc_return_value = _pVFCActivateSignal->GetVcStatus();
        if(vc_return_value != vc::RET_OK){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:%s, vc_return_value != vc::RET_OK", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return false;
        }
        std::shared_ptr<vc::ResVFCActivate> _resVFCActivate = _pVFCActivateSignal->GetData();
        if (GLY_TCAM_REMOTE_START_ACTIVE_VFC_ID == _resVFCActivate->id){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMTStartBasic:%s, VFC operater is ers's vfc", __FUNCTION__);
            if(GLY_TCAM_REMOTE_START_ACTIVE_VFC_SUCCESS == _resVFCActivate->resule)
            {//operate success
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMTStartBasic:%s, Success.", __FUNCTION__);
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMTStartBasic:%s, VFC operate failed", __FUNCTION__);
                HandleFailed(kConditionsNotCorrect);
                return false;
            }
        }
        return true;
    }

    bool RMTStartBasic::Handle_StartEngineSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartCommon::%s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<volvo_on_call::AppEngineStatResponseSignal> _res_signal =
                                        std::static_pointer_cast<volvo_on_call::AppEngineStatResponseSignal>(signal);
        if ((true == _res_signal->GetOperateStatus()) &&
             ( vc::EngSt1_RunngRemStrtd == _res_signal->GetEngStlWdSts()) &&
             (vc::ErsStrtApplSts_ErsStsRunng == _res_signal->GetErsStrtApplSts()))
        {//start engine success
            SetState(kHandleRequest);
            SendRequest(m_pGlyVdsRceSignal->GetRequestPayload());
            ret = true;
        } else {
            HandleFailed(kExecutionFailure);
        }
        return ret;
    }

    bool RMTStartBasic::Handle_DelayEngineSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, state: %d.", __FUNCTION__, GetState());
#if 0
        bool ret = false;
        std::shared_ptr<fsm::DelayEngRunngTimeSignal> _delay_signal =
                                                    std::static_pointer_cast<fsm::DelayEngRunngTimeSignal>(signal);
        vc::ReturnValue vc_return_value = _delay_signal->GetVcStatus();
        if (vc::RET_OK != vc_return_value)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. delay engine running time failed", __FUNCTION__);
            HandleFailed(kExecutionFailure);
            return ret;
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, reponse_value: %d.", __FUNCTION__, _delay_signal->GetData()->ersDelayTiCfm);
        if ( vc::NoYes1_Yes == _delay_signal->GetData()->ersDelayTiCfm)
        {// delay engine running time successful
            SetState(kHandleRequest);
            SendRequest(m_pGlyVdsRceSignal->GetClimateRequest());
            ret = true;
        } else {
            HandleFailed(kExecutionFailure);
        }
        return ret;
#elif 1
        SetState(kHandleRequest);
        SendRequest(m_pGlyVdsRceSignal->GetRequestPayload());
        return true;
#endif
    }

/*    bool RMTStartBasic::Handle_ResponseSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<RMT_ClimatizationRequest_t> request = m_pGlyVdsRceSignal->GetRequestPayload();
        switch (request->m_serviceType){
            case RMT_Climatization: {
#if 0
                std::shared_ptr<fsm::OperateRMTClimateSignal> _res_signal = std::static_pointer_cast<fsm::OperateRMTClimateSignal>(signal);
                vc::ReturnValue vc_return_value = _res_signal->GetVcStatus();
                if (vc::RET_OK != vc_return_value){
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, operate remote climate failed", __FUNCTION__);
                    HandleFailed(kExecutionFailure);
                    return ret;
                }
                if ((vc::OnOff1_On == _res_signal->GetData()->remStrtClimaActv) &&
                    (vc::ClimaWarn_NoWarn == _res_signal->GetData()->prkgClimaWarn))
                {
                    RMT_ClimatizationResponse_t res;
                    if (CMD_Start == m_cmd){
                        SetState(kWaitingStop);
                        m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request.m_duration_time), false);
                        res.m_operateSuccess = true;
                        res.m_active = _res_signal->GetData()->remStrtClimaActv;
                    } else if ((CMD_Stop == m_cmd) || (CMD_TimeoutStop == m_cmd) ){
                        SetState(kDone);
                        res.m_operateSuccess = true;
                    }
                    SendResponseToTSP(res);
                    ret = true;
                } else {
                    HandleFailed(kExecutionFailure);
                }
#elif 1
        RMT_ClimatizationResponse_t res;
        res.m_serviceType = RMT_Climatization;
        if (CMD_Start == m_cmd){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. start success, time: %d", __FUNCTION__, request->m_duration_time);
            SetState(kWaitingStop);
            m_RunngTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(request->m_duration_time), false);
            res.m_operateSuccess = true;
            res.m_resType = ResType_StrtStop;
            res.m_active = vc::OnOff1_On;
        } else if (CMD_Stop == m_cmd){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. tsp stop success", __FUNCTION__);
            SetState(kDone);
            res.m_resType = ResType_StrtStop;
            res.m_operateSuccess = true;
        } else if (CMD_TimeoutStop == m_cmd){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. CMD_TimeoutStop success", __FUNCTION__);
            SetState(kDone);
            res.m_resType = ResType_AutoStop;
            res.m_operateSuccess = true;
        }
        SendResponseToTSP(res);
        ret = true;
#endif
                break;
            }
            case RMT_SeatHeat:{
                break;
            }
            case RMT_SeatVentilation:{
                break;
            }
            case RMT_SteerWheelHeat:{
                break;
            }
            case RMT_NoService:
            default:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. NoService or Unknow ServiceType, Service(0~4): %d",
                                    __FUNCTION__, request->m_serviceType);
            }
        }
        return ret;
    }*/

    void RMTStartBasic::InitFlagsStruct()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        m_flags.m_UsageModeSignalFlage = false;
        m_flags.m_CarModeSignalFlags = false;
        m_flags.m_EngineLevelFlags = false;
        m_flags.m_EngineStatesFlags = false;
        return;
    }

    bool RMTStartBasic::Handle_GlyTimeout(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. Stat: %d", __FUNCTION__, GetState());
        if (m_TSPTimeoutId == signal->GetTransactionId()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, 
                                "RMTStartBasic::%s, the TSP response timeout.", __FUNCTION__);
            HandleFailed(kTimeout);
        }
        switch (GetState()){
            /**
             * \brief: when running timer is timeout, must stop remote seat-heating, seat-ventilation,
             *         climatization and steer-wheel-heating.
             * \steps: 
             *         1. check engine status, if engine is stoped, ingore this timeout event
             *         2. send stop request to CEM
             *         3. handle response signal and send reponse to TSP
            **/
            case kWaitingStop:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. send stop remote apps", __FUNCTION__);
                SetState(kTimeoutStop);
                std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
                std::make_shared<fsm::VehicleCommTransactionId>();
                MapTransactionId(vehicle_comm_request_id);
                if (m_igen.Request_GetEngineStates( vehicle_comm_request_id->GetId()) == vc::RET_OK)
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                        "RMTStartBasic::%s, SUCCESS", __FUNCTION__);
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                        "RMTStartBasic::%s, Failed", __FUNCTION__);
                    HandleFailed(kExecutionFailure);
                }
                break;
            }
            //TODO
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, 
                                "RMTStartBasic::%s, UNKnow state.", __FUNCTION__);
                break;
        }
        return true;
    }

    bool RMTStartBasic::Handle_DoneState()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, remote climate services finised.", __FUNCTION__);
        //Stop();
        return true;
    }

    bool RMTStartBasic::OperateEingne()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        bool ret = false;
        /*if ((false == m_flags.m_UsageModeSignalFlage) ||
            (false == m_flags.m_CarModeSignalFlags) ||
            (false == m_flags.m_EngineLevelFlags) ||
            (false == m_flags.m_EngineStatesFlags))
        {
            return ret;
        }*/

        if(CMD_Stop == m_pGlyVdsRceSignal->GetOperateCommand())
        {//stop climatization
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. Stop", __FUNCTION__);
            if ((false == m_flags.m_UsageModeSignalFlage) ||
                (false == m_flags.m_CarModeSignalFlags) ||
                (false == m_flags.m_EngineStatesFlags))
            {
                return ret;
            }
            /*std::shared_ptr<RMT_ClimatizationRequest_t> req = std::make_shared<RMT_ClimatizationRequest_t>();
            if(nullptr == req.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s. nullptr == req", __FUNCTION__);
                return false;
            }
            req->m_operateCmd = CMD_Stop;*/
            SendRequest(m_pGlyVdsRceSignal->GetRequestPayload());
        } else  if(CMD_Start == m_pGlyVdsRceSignal->GetOperateCommand()){ //start climatization
            if ((false == m_flags.m_UsageModeSignalFlage) ||
                (false == m_flags.m_CarModeSignalFlags) ||
                (false == m_flags.m_EngineLevelFlags) ||
                (false == m_flags.m_EngineStatesFlags))
            {
                return ret;
            }
            SetState(kOperateEgnine);
            if (( vc::ErsStrtApplSts_ErsStsRunng == m_ErsStartApplSts ) &&
            ( vc::EngSt1_RunngRemStrtd == m_EngStlWdSts ))
            {// engine is in remote strtd and it's running now
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, delay engine running timer.", __FUNCTION__);
                Request_DelayEnginneRunningTimer();
                ret = true;
            } else if (( vc::ErsStrtApplSts_ErsStsOff == m_ErsStartApplSts ) &&
                        ((vc::EngSt1_RunngRunng == m_EngStlWdSts) ||
                        (vc::EngSt1_RunngStb == m_EngStlWdSts)))
            {// engine si running now, but it's not in remote start status
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, Engine is running, but not in remote \
                            start status, executed failed", __FUNCTION__);
                HandleFailed(kExecutionFailure);
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "RMTStartBasic::%s, start engine.", __FUNCTION__);
                Request_StartEngine();
                ret = true;
            }
        }
        return ret;
    }

    void RMTStartBasic::HandleFailed(ASN_ErrorCode error_code)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        SetState(kDone);
        std::shared_ptr<RMT_ClimatizationRequest_t> request = m_pGlyVdsRceSignal->GetRequestPayload();
        RMT_ClimatizationResponse_t _response;
        /*if (CMD_Start == m_cmd){
            _response.m_resType = ResType_StrtStop;
        } else if (CMD_Stop == m_cmd){
            _response.m_resType = ResType_StrtStop;
        } else if (CMD_TimeoutStop == m_cmd){
            _response.m_resType = ResType_AutoStop;
        }*/
        _response.m_serviceType = request->m_serviceType;
        /*switch (request->m_serviceType){
            case RMT_Climatization:{
                _response.m_serviceType = RMT_Climatization;
                if (CMD_Start == m_cmd){
                    _response.m_resType = ResType_StrtStop;
                } else if (CMD_Stop == m_cmd){
                    _response.m_resType = ResType_StrtStop;
                } else if (CMD_TimeoutStop == m_cmd){
                    _response.m_resType = ResType_AutoStop;
                }
                break;
            }
            case RMT_SEATHEAT:{
                _response.m_serviceType = RMT_SEATHEAT;
                break;
            }
            case RMT_SEATVENTI:{
                _response.m_serviceType = RMT_SEATHEAT;
                break;
            }
            case RMT_STEERWHEELHEAT:{
                _response.m_serviceType = RMT_SEATHEAT;
                break;
            }
            default:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s, UNKNOW Service type.", __FUNCTION__);
                break;
            }
        }*/
        _response.m_resType = ResType_Err;
        _response.m_operateSuccess = false;
        _response.m_errorCode = error_code;
        SendResponseToTSP(_response);
    }
} //volvo_on_call