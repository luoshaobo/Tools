///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file remote_start_engine.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
//
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   07-Sep-2018
///////////////////////////////////////////////////////////////////

#include "remote_start/remote_start_engine.h"

#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include "applications_lcm/apps_lcm.h"

#include "dlt/dlt.h"  //log

#include <chrono>

namespace volvo_on_call
{
RMT_Engine::RMT_Engine():
        fsm::SmartTransaction(kNew),
        m_vocpersist(vocpersistdatamgr::VocPersistDataMgr::GetInstance()),
        m_operateCmd(vc::ErsCmd_ErsCmdNotSet),
        m_ersStrtAppStsInt(vc::ErsStrtApplSts_ErsStsOff),
        m_UsageModeFlag(false), m_CarModeFlag(false),
        m_EngineStateFlag(false), m_EngineLevelFlag(false), m_VFCFlag(false),
        m_finalizing(false), m_close_flag(true),
        m_create_thread_success(false), m_engine_mode(StartEngineMode_No)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine constructor start", __FUNCTION__);
    SignalFunction add_signal_deque =
            std::bind(&RMT_Engine::AddSignalDeque, this, std::placeholders::_1);
    SignalFunction handle_gly_request =
            std::bind(&RMT_Engine::HandleGlyRequest, this, std::placeholders::_1);
    SignalFunction handle_car_mode =
            std::bind(&RMT_Engine::HandleCarModeSignal, this, std::placeholders::_1);
    SignalFunction handle_usage_mode =
            std::bind(&RMT_Engine::HandleUsageModeSignal, this, std::placeholders::_1);
    SignalFunction Handle_EngineLevel_Signal =
            std::bind(&RMT_Engine::HandleEngineLevelSignal, this, std::placeholders::_1);
    SignalFunction handle_vfc_signal=
            std::bind(&RMT_Engine::HandleVFCSignal, this, std::placeholders::_1);
    SignalFunction handle_security_random_signal =
            std::bind(&RMT_Engine::HandleRMTEngineSecurityRandomSignal, this, std::placeholders::_1);
    SignalFunction handle_security_result_signal =
            std::bind(&RMT_Engine::HandleRMTEngineSecurityResultSignal, this, std::placeholders::_1);
    SignalFunction handle_start_signal =
            std::bind(&RMT_Engine::HandleRMTEngineSignal, this, std::placeholders::_1);
    SignalFunction handle_vehicleStates_signal =
            std::bind(&RMT_Engine::HandleRMTVehicleStatesSignal, this, std::placeholders::_1);
    SignalFunction handle_gly_timeout =
            std::bind(&RMT_Engine::GlyTimeout, this, std::placeholders::_1);

    StateMap state_map = {
            {kNew,                      {nullptr,
                                            {//{fsm::kVDServiceRes, handle_gly_request},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {volvo_on_call::kAppStartEngineRequestSignal, add_signal_deque},
                                            {fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::Signal::kTimeout, handle_gly_timeout}},
                                            {kWaitPreConCheckResponse, kWaitVFCResponse, kDone}}},
            {kWaitPreConCheckResponse,  {nullptr,
                                            {{fsm::Signal::kCarMode, handle_car_mode},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {fsm::Signal::kCarUsageMode, handle_usage_mode},
                                            {fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::Signal::kElectEngLvlSignal, Handle_EngineLevel_Signal},
                                            {fsm::Signal::kTimeout, handle_gly_timeout}},
                                            {kWaitVFCResponse, kWaitCheckStrtApplSts, kDone}}},
            {kWaitVFCResponse,          {nullptr,
                                            {{fsm::Signal::kVFCActivateSignal, handle_vfc_signal},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::Signal::kTimeout, handle_gly_timeout}},
                                            {kWaitCheckStrtApplSts, kDone}}},
            {kWaitCheckStrtApplSts,     {nullptr,
                                            {{fsm::Signal::kTimeout, handle_gly_timeout},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {fsm::Signal::kRMTEngineSignal, handle_start_signal}},
                                            {kWaitOperateEngineResponse, kWaitGetVehicleState, kDone}}},
            {kWaitOperateEngineResponse,{nullptr,
                                            {{fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::Signal::kRMTEngineSecurityRandom, handle_security_random_signal},
                                            {fsm::Signal::kRMTEngineSecurityResult, handle_security_result_signal},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {fsm::Signal::kTimeout, handle_gly_timeout}},
                                            {kWaitGetVehicleState, kDone}}},
            {kWaitGetVehicleState,      {nullptr,
                                            {{fsm::Signal::kRMTCarStatesSignal, handle_vehicleStates_signal},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::Signal::kCarUsageMode, handle_usage_mode},
                                            {fsm::Signal::kTimeout, handle_gly_timeout}},
                                            {kDone}}},
            {kDone,                     {nullptr,
                                            {//{fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::kVDServiceRes, add_signal_deque},
                                            {fsm::Signal::kRMTEngineSignal, handle_start_signal},
                                            {fsm::Signal::kVFCActivateSignal, handle_vfc_signal},
                                            {volvo_on_call::kAppStartEngineRequestSignal, add_signal_deque},
                                            {fsm::Signal::kTimeout, handle_gly_timeout}},
                                            {kWaitPreConCheckResponse}}}
    };
    SetStateMap(state_map);
    MapSignalType(fsm::kVDServiceRes);
    MapSignalType(fsm::Signal::kRMTEngineSignal);
    MapSignalType(fsm::Signal::kCarUsageMode);
    MapSignalType(fsm::Signal::kVFCActivateSignal);
    MapSignalType(fsm::Signal::kTimeout);
    MapSignalType(fsm::Signal::kRMTEngineSecurityRandom);
    MapSignalType(fsm::Signal::kRMTEngineSecurityResult);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine constructor end, add: %x", __FUNCTION__, this);
}

    RMT_Engine::~RMT_Engine()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine destructor, add: %x", __FUNCTION__, this);
        TerminateSignalThread();
    }

    std::shared_ptr<GlyVdsResSignal> RMT_Engine::CreateGlyVdsResSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine::CreateGlyVdsResSignal", __FUNCTION__);
        return std::dynamic_pointer_cast<volvo_on_call::GlyVdsResSignal>(signal);
    }

    bool RMT_Engine::Request_Preconditions()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s.", __FUNCTION__);
        bool ret = false;
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        SetState(kWaitPreConCheckResponse);
        switch (m_operateCmd)
        {
                case vc::ErsCmd_ErsCmdOn://operate start engine
                {
                    if ((igen.Request_CarMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
                            (igen.Request_CarUsageMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
                            (igen.Request_ElectEnergyLevel( vehicle_comm_request_id->GetId()) == vc::RET_OK))
                    {
                        ret = true;
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                            "RMT_Engine::%s, SUCCESS, current state: %s", __FUNCTION__, (GetRMT_EngineState()).c_str());
                    }
                }
                break;
            case vc::ErsCmd_ErsCmdOff://operate stop engine
                {
                    if ((igen.Request_CarUsageMode( vehicle_comm_request_id->GetId()) == vc::RET_OK) &&
                            (igen.Request_GetEngineStates( vehicle_comm_request_id->GetId()) == vc::RET_OK))
                    {
                        ret = true;
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                                            "RMT_Engine::%s, SUCCESS, current state: %s", __FUNCTION__, (GetRMT_EngineState()).c_str());
                    }
                }
                break;
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s. UNKnow operation", __FUNCTION__);
                ret = true;
                break;
        }
        if(!ret){ // send request error
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                "RMT_Engine::%s, FAILED, get CarMode, UsageMode or EngineStates, aborting res.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
        }
        return ret;
    }

    bool RMT_Engine::Request_VFCActive(bool active_vfc)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s,", __FUNCTION__);
        m_CarModeFlag = false;
        m_UsageModeFlag = false;
        m_EngineLevelFlag = false;
        m_VFCFlag = false;
        unsigned int m_VFCTimeout;

        if(active_vfc){
            m_VFCFlag = true;
        }

        bool ret = m_vocpersist.GetData(vocpersistdatamgr::ERS_VFC_WAKEUP_TIME, m_VFCTimeout);
        if (!ret){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , failed, using default vfc-timeout time value: 3s", __FUNCTION__);
            m_VFCTimeout = RMT_RES_VFC_WAKEUP_TIME_DEFAULT;
        } else {
            if ((RMT_RES_VFC_WAKEUP_TIME_MIN > m_VFCTimeout)
                && (RMT_RES_VFC_WAKEUP_TIME_MAX < m_VFCTimeout)){
                m_VFCTimeout = RMT_RES_VFC_WAKEUP_TIME_DEFAULT;
            }
        }

        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        vc::ReqVFCActivate request;
        request.id = RMT_ERS_ACTIVE_VFC_ID;
        request.type = active_vfc;
        if(active_vfc){
            SetState(kWaitVFCResponse);
        }
        if (igen.Request_VFCActivate(&request, vehicle_comm_request_id->GetId()) == vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, m_VFCTimeout timer: %ds", __FUNCTION__, m_VFCTimeout);
            m_timeoutTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(m_VFCTimeout), false);
            MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                "RMT_Engine: %s, success to requests for VFC %s.", __FUNCTION__, (active_vfc)?"Active":"Inactive");
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                "RMT_Engine: %s, failed to requests for VFC %s.", __FUNCTION__, (active_vfc)?"Active":"Inactive");
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
        }
        return true; //wake up CAN successful
    }

    bool RMT_Engine::Request_EngineStates()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s.", __FUNCTION__);
        //get ers_status_update_time timer
        unsigned int ers_status_update_time = RMT_RES_STATUS_UPDATE_TIME_DEFAULT;
        bool ret = m_vocpersist.GetData(vocpersistdatamgr::ERS_RESPONSE_TIME, ers_status_update_time);
        if(false == ret ){// get timer failed
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , failed, using default value: 15s", __FUNCTION__);
            ers_status_update_time = RMT_RES_STATUS_UPDATE_TIME_DEFAULT;
        } else { //get timer success
            if((RMT_RES_STATUS_UPDATE_TIME_MIN > ers_status_update_time)
                && (ers_status_update_time < RMT_RES_STATUS_UPDATE_TIME_MAX)){//out of range 0 ~ 255s
                ers_status_update_time = RMT_RES_STATUS_UPDATE_TIME_DEFAULT;
            }
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , ers_status_update_time: %ds", __FUNCTION__, ers_status_update_time);
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        SetState(kWaitCheckStrtApplSts);
        if (vc::RET_OK == igen.Request_GetEngineStates( vehicle_comm_request_id->GetId()))
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, State: %s", __FUNCTION__, GetRMT_EngineState().c_str());
            m_timeoutTransactionId = RequestTimeout(static_cast<std::chrono::seconds>(ers_status_update_time), false);
            MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
        }else { // send request failed
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, failed to get engine states", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
            return false;
        }
        return true;
    }

    bool RMT_Engine::Request_StartEngine(std::shared_ptr<ErsRequest_t> ersRequest)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s ", __FUNCTION__);
        // get RMT_RESPONSE_TIME timeout timer
        unsigned int ers_response_time = RMT_RESPONSE_TIME_DEFAULTE;
        bool ret = m_vocpersist.GetData(vocpersistdatamgr::ERS_RESPONSE_TIME, ers_response_time);
        if(false == ret ){//// get timer failed
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, FAILED, using default value: 15s", __FUNCTION__);
            ers_response_time = RMT_RESPONSE_TIME_DEFAULTE;
        } else {
            if((RMT_RESPONSE_TIME_MIN > ers_response_time)
                && (ers_response_time < RMT_RESPONSE_TIME_MAX)){//out of range 0 ~ 255s
                ers_response_time = RMT_RESPONSE_TIME_DEFAULTE;
            }
        }

        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, RMT_RESPONSE_TIM: %d", __FUNCTION__, ers_response_time);
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        vc::Req_RMTEngine _reqRMTEngine;
        if (vc::ErsCmd_ErsCmdOn == (vc::ErsCmd)ersRequest->ersCmd){ //start ers
            _reqRMTEngine.ersCmd     = vc::ErsCmd_ErsCmdOn;
            _reqRMTEngine.ersRunTime = ersRequest-> ersRunTime;
        } else if(vc::ErsCmd_ErsCmdOff == (vc::ErsCmd)ersRequest->ersCmd){
            //TODO stop ers
            _reqRMTEngine.ersCmd     = vc::ErsCmd_ErsCmdOff;
            _reqRMTEngine.ersRunTime = RMT_ENGINE_START_DEFAULT_TIME;
        } else {
            _reqRMTEngine.ersCmd     = vc::ErsCmd_ErsCmdNotSet;
            _reqRMTEngine.ersRunTime = RMT_ENGINE_START_DEFAULT_TIME;
        }
        /*TODO, because the Authentication certificate has not done, so give it a value.
         *When doing Autenticatin certificate, will update htese value*/
        _reqRMTEngine.imobVehRemReqCmd = 4;
        _reqRMTEngine.imobVehDataRemReq0 = 1;
        _reqRMTEngine.imobVehDataRemReq1 = 1;
        _reqRMTEngine.imobVehDataRemReq2 = 1;
        _reqRMTEngine.imobVehDataRemReq3 = 1;
        _reqRMTEngine.imobVehDataRemReq4 = 1;
        _reqRMTEngine.imobVehRemTmrOrSpdLim = 1;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , _reqRMTEngine.ersCmd: %d", __FUNCTION__, _reqRMTEngine.ersCmd);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , _reqRMTEngine.ersRunTime: %d mins", __FUNCTION__, _reqRMTEngine.ersRunTime);
        SetState(kWaitOperateEngineResponse);
        if(vc::RET_OK == igen.Request_SendRMTEngine(&_reqRMTEngine, vehicle_comm_request_id->GetId())){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, Request_SendRMTEngine OK", __FUNCTION__);
            if (vc::ErsCmd_ErsCmdNotSet != ersRequest->ersCmd){ // add timeout
                m_timeoutTransactionId = RequestTimeout(std::chrono::seconds(ers_response_time), false);
                MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                    "RMT_Engine::%s, Request_SendRMTEngine OK. State: %s.", __FUNCTION__, GetRMT_EngineState().c_str());
            }
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, Request_SendRMTEngine Failed, aborting res.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
        }
        return true;
    }

    bool RMT_Engine::Request_OperateEngine()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s ", __FUNCTION__);
        // get RMT_RESPONSE_TIME timeout timer
        unsigned int ers_response_time = RMT_RESPONSE_TIME_DEFAULTE;
        bool ret = m_vocpersist.GetData(vocpersistdatamgr::ERS_RESPONSE_TIME, ers_response_time);
        if(false == ret ){//// get timer failed
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, FAILED, using default value: 15s", __FUNCTION__);
            ers_response_time = RMT_RESPONSE_TIME_DEFAULTE;
        } else {
            if((RMT_RESPONSE_TIME_MIN > ers_response_time)
                && (ers_response_time < RMT_RESPONSE_TIME_MAX)){//out of range 0 ~ 255s
                ers_response_time = RMT_RESPONSE_TIME_DEFAULTE;
            }
        }

        std::shared_ptr<ErsRequest_t> ersRequest = m_pGlyVdsResSignal->GetRequestPayload();
        vc::Req_RMTEngine _reqRMTEngine;
        _reqRMTEngine.ersCmd     = ersRequest->ersCmd;
        _reqRMTEngine.ersRunTime = ersRequest-> ersRunTime;
        /*TODO, because the Authentication certificate has not done, so give it a value.
         *When doing Autenticatin certificate, will update htese value*/
        _reqRMTEngine.imobVehRemReqCmd = 4;
        _reqRMTEngine.imobVehDataRemReq0 = 0;
        _reqRMTEngine.imobVehDataRemReq1 = 0;
        _reqRMTEngine.imobVehDataRemReq2 = 0;
        _reqRMTEngine.imobVehDataRemReq3 = 0;
        _reqRMTEngine.imobVehDataRemReq4 = 0;
        _reqRMTEngine.imobVehRemTmrOrSpdLim = 0;
        //Send Request
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , _reqRMTEngine.ersCmd: %d", __FUNCTION__, _reqRMTEngine.ersCmd);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s , _reqRMTEngine.ersRunTime: %d mins", __FUNCTION__, _reqRMTEngine.ersRunTime);
        SetState(kWaitOperateEngineResponse);
        if(vc::RET_OK == igen.Request_SendRMTEngine(&_reqRMTEngine, vehicle_comm_request_id->GetId())){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, Request_SendRMTEngine OK", __FUNCTION__);
            if (vc::ErsCmd_ErsCmdNotSet != ersRequest->ersCmd){ // add timeout
                m_timeoutTransactionId = RequestTimeout(std::chrono::seconds(ers_response_time), false);
                MapTransactionId(m_timeoutTransactionId.GetSharedCopy());
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                    "RMT_Engine::%s, Request_SendRMTEngine OK. State: %s.", __FUNCTION__, GetRMT_EngineState().c_str());
            }
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, Request_SendRMTEngine Failed, aborting res.", __FUNCTION__);
            if (vc::ErsCmd_ErsCmdNotSet != ersRequest->ersCmd){
                return false;
            }
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
        }
        return true;
    }

    bool RMT_Engine::Request_RMTEngineSecurityReponse(std::shared_ptr<vc::Res_RMTEngineSecurityRandom> pRandom)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s.", __FUNCTION__);
        if(AvlSts1_Avl == pRandom->imobRemMgrChk){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, Security authentication AvlSts1_Avl success", __FUNCTION__);
            std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
                std::make_shared<fsm::VehicleCommTransactionId>();
            MapTransactionId(vehicle_comm_request_id);
            fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
            vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
            vc::Req_RMTEngineSecurityResponse req;
            req.imobVehRemReqCmd = 5;
            req.imobVehDataRemReq0 = 0;
            req.imobVehDataRemReq1 = 0;
            req.imobVehDataRemReq2 = 0;
            req.imobVehDataRemReq3 = 0;
            req.imobVehDataRemReq4 = 0;
            req.imobVehRemTmrOrSpdLim = 0;
            if(vc::RET_OK == igen.Request_RmtEngineSecurityRespose(&req, vehicle_comm_request_id->GetId())){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, send to VUC success", __FUNCTION__ , GetRMT_EngineState().c_str());
            } else {
                SetState(kDone);
                ErsResponse_t _serviceResult;
                _serviceResult.operationSucceeded = false;
                _serviceResult.errorCode = executionFailure;
                SendError(_serviceResult);
            }
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, Security authentication AvlSts1_Avl failed, imobRemMgrChk: %d",
                        __FUNCTION__, pRandom->imobRemMgrChk);
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
        }
        return true;
    }


    bool RMT_Engine::Request_VehicleStates()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s.", __FUNCTION__);
        std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
            std::make_shared<fsm::VehicleCommTransactionId>();
        MapTransactionId(vehicle_comm_request_id);
        fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
        vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();
        bool ret = false;
        SetState(kWaitGetVehicleState);
        switch (m_operateCmd)
        {
            case vc::ErsCmd_ErsCmdOn:{
                    if (vc::RET_OK == igen.Request_GetRMTStatus( vehicle_comm_request_id->GetId()))
                    {
                        ret = true;
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, State: %s, start cmd", __FUNCTION__ , GetRMT_EngineState().c_str());
                    }
                }
                break;
            case vc::ErsCmd_ErsCmdOff:
                    if(vc::RET_OK == igen.Request_CarUsageMode(vehicle_comm_request_id->GetId())){
                        ret = true;
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, State: %s, stop cmd", __FUNCTION__ , GetRMT_EngineState().c_str());
                    }
                break;
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, State: %s, Cmd: ", __FUNCTION__ , GetRMT_EngineState().c_str(), m_operateCmd);
                ret = true;
                break;
        }
        if(!ret){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                "RMT_Engine::%s, Request Failed, aborting res.", __FUNCTION__);
            RemoveTimeout(m_timeoutTransactionId);
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
        }
        return ret;
    }

    /**
     * \brief steps:  by TSP server
     *      start engine:
     *          1. Check precondition
     *          2. VFC active, Nad -> Vuc
     *          3. Check ErsStrtApplSts
     *          4. Send start request, Nad -> Vuc
     *          5. Handle response, Vuc -> Nad
     *          6. Send result to TSP, Nad -> TSP
     *      stop engine:
     *          1. Check precondition
     *          2. Check ErsStrtApplSts
     *          3. Send stop request Nad -> Vuc
     *          4. Handle response, Vuc -> Nad
     *          5. Send response to TSP, Nad -> TSP
     *
     *      by other application:
     *          1. VFC active
     *          2. Check ErsStrtApplSts
     *          3. Send start request, Nad -> Vuc
     *          4. Handle response, Vuc -> Nad
     *          5. Notify other application
     */
    bool RMT_Engine::HandleGlyRequest(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine::HandleGlyRequest", __FUNCTION__);
        std::shared_ptr<ErsRequest_t> ersRequest = nullptr;
        switch(signal->GetSignalType()){
            case VocInternalSignalTypes::kAppStartEngineRequestSignal:{ //<*APP start engine
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, engine started by other applications", __FUNCTION__);
                m_engine_mode = StartEngineMode_APPS;
                m_pAppRequestSignal.reset();
                m_pAppRequestSignal = std::dynamic_pointer_cast<volvo_on_call::AppEngineStatRequestSignal>(signal);
                ersRequest = m_pAppRequestSignal->GetRequestPayload();
                break;
            }
            case fsm::kVDServiceRes:{ // <*TSP start engine
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, engine started by TSP Server", __FUNCTION__);
                m_engine_mode = StartEngineMode_TSP;
                m_pGlyVdsResSignal.reset();
                m_pGlyVdsResSignal = std::dynamic_pointer_cast<volvo_on_call::GlyVdsResSignal>(signal);
                ersRequest = m_pGlyVdsResSignal->GetRequestPayload();
                break;
            }
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, UNKNOW Mode type.", __FUNCTION__);
                return false;
        }

        unsigned int ers_response_vaild_time = RMT_ERS_RESPONSE_VALID_TIME_DEFAULT;
        bool ret = m_vocpersist.GetData(vocpersistdatamgr::ERS_RESPONSE_VALID_TIME, ers_response_vaild_time);
        if(false == ret ){//// get timer failed
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, FAILED, using default value: 15s", __FUNCTION__);
            ers_response_vaild_time = RMT_ERS_RESPONSE_VALID_TIME_DEFAULT;
        } else {
            if((RMT_ERS_RESPONSE_VALID_TIME_MIN > ers_response_vaild_time) && (ers_response_vaild_time < RMT_ERS_RESPONSE_VALID_TIME_MAX)){
                ers_response_vaild_time = RMT_ERS_RESPONSE_VALID_TIME_DEFAULT;
            }
        }

        if (nullptr == ersRequest.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, ersRequest.get() == nullptr", __FUNCTION__);
            return true;
        }

        switch(ersRequest->ersCmd)
        {
            case vc::ErsCmd_ErsCmdOn:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine::HandleGlyRequest, ersCmd: start ", __FUNCTION__);
                m_operateCmd = vc::ErsCmd_ErsCmdOn;
                break;
            case vc::ErsCmd_ErsCmdOff:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine::HandleGlyRequest, ersCmd: terminate ", __FUNCTION__);
                m_operateCmd = vc::ErsCmd_ErsCmdOff;
                break;
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_Engine::%s, UNKOWN operate command. ", __FUNCTION__);
                return false;
        }

        switch (m_engine_mode){
            case StartEngineMode_APPS:{ // application start engine
                Request_VFCActive(true);
                break;
            }
            case StartEngineMode_TSP:{ // TSP server start engine
                m_TSPTimeoutId = RequestTimeout(std::chrono::seconds(ers_response_vaild_time), false);
                MapTransactionId(m_TSPTimeoutId.GetSharedCopy());
                Request_Preconditions();
                break;
            }
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, UNKNOW Mode type", __FUNCTION__);
                return false;
            }

        return true;
    }

    bool RMT_Engine::HandleStartSignalRequest(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s RMT_Engine::HandleStartSignal", __FUNCTION__);

        m_pGlyVdsResSignal = std::dynamic_pointer_cast<volvo_on_call::GlyVdsResSignal>(signal);
        std::shared_ptr<ErsRequest_t> ersRequest = m_pGlyVdsResSignal->GetRequestPayload();
        if(!Request_VFCActive(true)){ //failed
            //SendError();
            return false;
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s end", __FUNCTION__);
        return true;
    }

    bool RMT_Engine::HandleUsageModeSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s", __FUNCTION__);

        std::shared_ptr<fsm::CarUsageModeSignal> car_usage_mode_response = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);
        vc::ReturnValue vc_return_value = car_usage_mode_response->GetVcStatus();
        if (vc::RET_OK != vc_return_value){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, Failed vc_return_value != vc::RET_OK.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
            return false;
        }
        vc::CarUsageModeState car_usage_mode_return = car_usage_mode_response->GetData()->usagemode;

        if (kWaitPreConCheckResponse == GetState()){
            switch (m_operateCmd)
            {
                case vc::ErsCmd_ErsCmdOn:
                    {
                        if ((vc::CAR_ABANDONED == car_usage_mode_return) ||
                            (vc::CAR_INACTIVE == car_usage_mode_return)){
                            if(m_CarModeFlag && m_EngineLevelFlag){
                                Request_VFCActive(true);
                            } else {
                                m_UsageModeFlag = true;
                            }
                        } else {
                            SetState(kDone);
                            ErsResponse_t _serviceResult;
                            _serviceResult.operationSucceeded = false;
                            _serviceResult.errorCode = conditionsNotCorrect;
                            SendError(_serviceResult);
                        }
                    }
                    break;
                case vc::ErsCmd_ErsCmdOff:
                    {
                        if (vc::CAR_INACTIVE == car_usage_mode_return){
                            if(m_EngineStateFlag){
                                Request_VFCActive(true);
                                //Request_EngineStates();
                            } else {
                                m_UsageModeFlag = true;
                            }
                        } else { //does not satisfy stop-condition
                            SetState(kDone);
                            ErsResponse_t _serviceResult;
                            _serviceResult.operationSucceeded = false;
                            _serviceResult.errorCode = conditionsNotCorrect;
                            SendError(_serviceResult);
                        }
                    }
                    break;
                default:
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s. UNKnow operation", __FUNCTION__);
                    break;
            }
        }else if (kWaitGetVehicleState == GetState()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s. CarUsageModeState: %d ", __FUNCTION__, car_usage_mode_return);
            SendStopSuccessful(car_usage_mode_return);
        }
        return true;
    }

    bool RMT_Engine::HandleCarModeSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, state: %d", __FUNCTION__, GetState());

        std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);
        vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, Failed vc_return_value != vc::RET_OK.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
            return false;
        }
        vc::CarModeState car_mode_return = car_mode_response->GetData()->carmode;
        if (kWaitPreConCheckResponse == GetState())
        {
            if (car_mode_return == vc::CAR_NORMAL)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMT_Engine::HandleCarModeSignal, car mode OK");
                if(m_UsageModeFlag && m_EngineLevelFlag){
                    Request_VFCActive(true);
                } else {
                    m_CarModeFlag = true;
                }
            } else {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMT_Engine::HandleCarModeSignal, car mode NOK");
                    m_CarModeFlag = false;
                SetState(kDone);
                ErsResponse_t _serviceResult;
                _serviceResult.operationSucceeded = false;
                _serviceResult.errorCode = conditionsNotCorrect;
                SendError(_serviceResult);
                return false;
            }
        }
        return true;
    }


    bool RMT_Engine::HandleEngineLevelSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMTStartBasic::%s.", __FUNCTION__);
        m_EngineLevelFlag = false;
        bool ret = false;
        std::shared_ptr<fsm::ElectEngLvlSignal> m_engine_level =  std::static_pointer_cast<fsm::ElectEngLvlSignal>(signal);
        vc::ReturnValue vc_return_value = m_engine_level->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "RMTStartBasic::%s, Get engine Level Failed.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t _serviceResult;
            _serviceResult.operationSucceeded = false;
            _serviceResult.errorCode = executionFailure;
            SendError(_serviceResult);
            return ret;
        }
        if (kWaitPreConCheckResponse == GetState()){
            if (vc::ENERGY_LVL_NORMAL == m_engine_level->GetData()->level)
            {
                if(m_CarModeFlag && m_UsageModeFlag){
                    Request_VFCActive(true);
                } else {
                    m_EngineLevelFlag = true;
                }
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, Engine Level != vc::ENERGY_LVL_NORMAL", __FUNCTION__);
                m_CarModeFlag = false;
                SetState(kDone);
                ErsResponse_t _serviceResult;
                _serviceResult.operationSucceeded = false;
                _serviceResult.errorCode = conditionsNotCorrect;
                SendError(_serviceResult);
                return false;
            }
        }
        return true;
    }


    bool RMT_Engine::HandleVFCSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);

        if(kWaitVFCResponse == GetState() || kDone == GetState()){
            RemoveTimeout(m_timeoutTransactionId);
            std::shared_ptr<fsm::VFCActivateSignal> _pVFCActivateSignal = std::static_pointer_cast<fsm::VFCActivateSignal>(signal);
            vc::ReturnValue vc_return_value = _pVFCActivateSignal->GetVcStatus();
            if(vc_return_value != vc::RET_OK){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:%s, vc_return_value != vc::RET_OK", __FUNCTION__);
                SetState(kDone);
                ErsResponse_t ers_response;
                ers_response.operationSucceeded = false;
                ers_response.errorCode = executionFailure;
                SendError(ers_response);
                return false;
            }
            std::shared_ptr<vc::ResVFCActivate> _resVFCActivate = _pVFCActivateSignal->GetData();
            if (RMT_ERS_ACTIVE_VFC_ID == _resVFCActivate->id){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:%s, VFC operater is ers's vfc", __FUNCTION__);
                if(RMT_VFC_SUCCESS == _resVFCActivate->resule){//operate success
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:%s, Success; current state: %s",
                                            __FUNCTION__, GetRMT_EngineState().c_str());
                    if (kWaitVFCResponse == GetState()){
                         Request_EngineStates();
                    } else { // inactive VFC
                        SetState(kDone);
                    }
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:%s, VFC operate failed", __FUNCTION__);
                    SetState(kDone);
                    ErsResponse_t ers_response;
                    ers_response.operationSucceeded = false;
                    ers_response.errorCode = executionFailure;
                    SendError(ers_response);
                    return false;
                }
            }
        }
        return true;
    }

    bool RMT_Engine::HandleRMTEngineSecurityRandomSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s", __FUNCTION__);
        std::shared_ptr<fsm::RMTEngineSecurityRandomSignal> _pRandomSignal = 
                            std::static_pointer_cast<fsm::RMTEngineSecurityRandomSignal>(signal);
        vc::ReturnValue vc_return_value = _pRandomSignal->GetVcStatus();
        if (vc::RET_OK != vc_return_value){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:: %s , vc_return_value != vc::RET_OK.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
            return false;
        }
        std::shared_ptr<vc::Res_RMTEngineSecurityRandom> _random = _pRandomSignal->GetData();
        if (AvlSts1_Avl == (ImobRemMgrChk)_random->imobRemMgrChk){
            Request_RMTEngineSecurityReponse(_random);
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:: %s, AvlSts1_Avl != (ImobRemMgrChk)_pRandomSignal->imobRemMgrChk.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
            return false;
        }
        return true;
    }

    bool RMT_Engine::HandleRMTEngineSecurityResultSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s", __FUNCTION__);
        std::shared_ptr<fsm::RMTEngineSercurityResultSignal> _pResultSignal =
                            std::static_pointer_cast<fsm::RMTEngineSercurityResultSignal>(signal);
        vc::ReturnValue vc_return_value = _pResultSignal->GetVcStatus();
        if (vc::RET_OK != vc_return_value){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:: %s , vc_return_value != vc::RET_OK.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
            return false;
        }
        std::shared_ptr<vc::Res_RMTEngineSecurityResult> _result = _pResultSignal->GetData();
        if (ImobVehRemMgrSts_ImobnRemMgrSts == (ImobVehRemMgrSts)_result->imobVehRemMgrSts){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:: %s , security Authentication success", __FUNCTION__);
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:: %s , security Authentication failed", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
            return false;
        }
        return true;
    }

    bool RMT_Engine::HandleRMTEngineSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s", __FUNCTION__);

        std::shared_ptr<fsm::RMTEngineSignal> _pRMTEngineSignal_ =  std::static_pointer_cast<fsm::RMTEngineSignal>(signal);
        vc::ReturnValue vc_return_value = _pRMTEngineSignal_->GetVcStatus();
        if (vc_return_value != vc::RET_OK)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine:: %s , vc_return_value != vc::RET_OK.", __FUNCTION__);
            SetState(kDone);
            ErsResponse_t ers_response;
            ers_response.operationSucceeded = false;
            ers_response.errorCode = executionFailure;
            SendError(ers_response);
            return false;
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, current state: %s , ErsStrtRes: %d ", __FUNCTION__, GetRMT_EngineState().c_str(), _pRMTEngineSignal_->GetData()->ersStrtRes);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, current state: %s , EngStlWdSts: %d ", __FUNCTION__, GetRMT_EngineState().c_str(), _pRMTEngineSignal_->GetData()->engSt1WdSts);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, current state: %s , ErsStrtApplSts: %d ", __FUNCTION__, GetRMT_EngineState().c_str(), _pRMTEngineSignal_->GetData()->ersStrtApplSts);
        //m_ersStrtAppStsInt = _pRMTEngineSignal_->GetData()->ersStrtApplSts;

        switch (GetState()){
            case kWaitPreConCheckResponse:{ // as stop engine's precondition
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, state: kWaitPreConCheckResponse",
                                __FUNCTION__, GetRMT_EngineState().c_str());
                //std::shared_ptr<fsm::RMTEngineSignal> _pRMTEngineSignal_ =  std::static_pointer_cast<fsm::RMTEngineSignal>(signal);
                /** @brief engStlWdSts is invalid in E3 version, but it'll be used in E3U
                 *if ((vc::EngSt1_RunngRemStrtd == _pRMTEngineSignal_->GetData()->engSt1WdSts)
                 *   && (vc::ErsStrtApplSts_ErsStsOff != _pRMTEngineSignal_->GetData()->ersStrtApplSts))*/
                if ((vc::ErsStrtApplSts_ErsStsOff != _pRMTEngineSignal_->GetData()->ersStrtApplSts))
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, state: %d, success", __FUNCTION__, GetRMT_EngineState().c_str());
                    if(m_UsageModeFlag){
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, go Request_EngineStates", __FUNCTION__);
                        Request_VFCActive(true);
                        //Request_EngineStates();
                    } else {
                        m_EngineStateFlag = true;
                    }
                } else { //does not satisfy stop-condition
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, state: kWaitPreConCheckResponse, Failed", __FUNCTION__);
                    SetState(kDone);
                    ErsResponse_t _serviceResult;
                    _serviceResult.operationSucceeded = false;
                    _serviceResult.errorCode = conditionsNotCorrect;
                    SendError(_serviceResult);
                }
                break;
            }
            case kWaitOperateEngineResponse:{ // wait request's response
                Handle_WaitOperateEngineResponseState(_pRMTEngineSignal_);
                break;
            }
            case kWaitCheckStrtApplSts:{ // to check engine status
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, current state: %s", __FUNCTION__, GetRMT_EngineState().c_str());
                break;
            }
            case kNew:
            case kWaitVFCResponse:
            case kWaitGetVehicleState:
            case kDone:
            default:
            {
                if(_pRMTEngineSignal_->GetData()->ersStrtApplSts == m_ersStrtAppStsInt){
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                            "RMT_Engine::%s, Engine stauts has not been changed", __FUNCTION__); 
                }
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                        "RMT_Engine::%s, Engine stauts has been changed", __FUNCTION__);

                ErsResponse_t serviceResult;
                serviceResult.type_ = ErsResponseType_Notify;
                serviceResult.engine_state = _pRMTEngineSignal_->GetData()->ersStrtApplSts;
                m_pGlyVdsResSignal->SetResultPayload(serviceResult);
                if((vc::ErsStrtApplSts_ErsStsOff == serviceResult.engine_state) &&
                    (vc::ErsStrtRes_ErsStrtNotSet != _pRMTEngineSignal_->GetData()->ersStrtRes)){
                        serviceResult.vehicleErrorCode = _pRMTEngineSignal_->GetData()->ersStrtRes;
                }
                fsm::VocmoSignalSource vocmo_signal_source;
                if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsResSignal))
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_Engine::%s, message sent ok.", __FUNCTION__);
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_Engine::%s "
                        "sync engine state Failed.", __FUNCTION__);
                }
                break;
            }
        }
        m_ersStrtAppStsInt = _pRMTEngineSignal_->GetData()->ersStrtApplSts;
        return true;
    }

    bool RMT_Engine::HandleRMTVehicleStatesSignal(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s ", __FUNCTION__);
        bool ret = false;
        if (kWaitGetVehicleState == GetState())
        {
            RemoveTimeout(m_timeoutTransactionId);
            std::shared_ptr<fsm::RMTCarStatesSignal> _pRMTCarStatesSignal = std::static_pointer_cast<fsm::RMTCarStatesSignal>(signal);
            vc::ReturnValue vc_return_value = _pRMTCarStatesSignal->GetVcStatus();
            if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, Failed", __FUNCTION__);
                SetState(kDone);
                ErsResponse_t ers_response;
                ers_response.operationSucceeded = false;
                ers_response.errorCode = executionFailure;
                SendError(ers_response);
                return false;
            }
            std::shared_ptr<vc::ResRMTStatus> _pResRMTStatus = _pRMTCarStatesSignal->GetData();
            if(!_pResRMTStatus){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, get car state failed", __FUNCTION__);
                //TODO send get car state to vehicle_commo(data-storage)
            } else {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s,get car states success", __FUNCTION__);
                ret = true;
            }
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: vehSpdIndcd:  %d!", __FUNCTION__, _pResRMTStatus->vehSpdIndcd);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: vehSpdIndcdQly:  %d!", __FUNCTION__, _pResRMTStatus->vehSpdIndcdQly);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorPassLockSts:  %d!", __FUNCTION__, _pResRMTStatus->doorPassLockSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorPassSts:  %d!", __FUNCTION__, _pResRMTStatus->doorPassSts);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorLeReLockSrs:  %d!", __FUNCTION__, _pResRMTStatus->doorLeReLockSrs);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorLeReSts:  %d!", __FUNCTION__, _pResRMTStatus->doorLeReSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorDrvrSts:  %d!", __FUNCTION__, _pResRMTStatus->doorDrvrSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorDrvrLockSts:  %d!", __FUNCTION__, _pResRMTStatus->doorDrvrLockSts);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorRiReLockSts:  %d!", __FUNCTION__, _pResRMTStatus->doorRiReLockSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: doorRiReSts:  %d!", __FUNCTION__, _pResRMTStatus->doorRiReSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: lockgCenSts:  %d!", __FUNCTION__, _pResRMTStatus->lockgCenSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: engOilLvlSts:  %d!", __FUNCTION__, _pResRMTStatus->engOilLvlSts);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: epbSts:  %d!", __FUNCTION__, _pResRMTStatus->epbSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: fuLvlIndcd:  %d!", __FUNCTION__, _pResRMTStatus->fuLvlIndcd);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: fuLvlLoIndcn:  %d!", __FUNCTION__, _pResRMTStatus->fuLvlLoIndcn);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: fulLvlLoWarn:  %d!", __FUNCTION__, _pResRMTStatus->fulLvlLoWarn);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: sunroofPosnSts:  %d!", __FUNCTION__, _pResRMTStatus->sunroofPosnSts);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: winPosnStsAtDrv:  %d!", __FUNCTION__, _pResRMTStatus->winPosnStsAtDrv);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: winPosnStsAtPass:  %d!", __FUNCTION__, _pResRMTStatus->winPosnStsAtPass);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: winPosnStsReLe:  %d!", __FUNCTION__, _pResRMTStatus->winPosnStsReLe);

            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: winPosnStsReRi:  %d!", __FUNCTION__, _pResRMTStatus->winPosnStsReRi);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine: %s: ventilateStatus:  %d!", __FUNCTION__, _pResRMTStatus->ventilateStatus);
            SendSuccessful(_pResRMTStatus);
        }else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, UNKNOW VehicleStatesSignal", __FUNCTION__);
        }
        return ret;
    }

    bool RMT_Engine::GlyTimeout(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);

        ErsResponse_t serviceResult;
        serviceResult.operationSucceeded = false;
        serviceResult.errorCode = timeout;
        if(m_TSPTimeoutId == signal->GetTransactionId()){ //TSP-request timeout
            SetState(kDone);
            SendError(serviceResult);
            return false;
        }
        switch (GetState()){
            case kWaitCheckStrtApplSts:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, kWaitCheckStrtApplSts timeout occured", __FUNCTION__);
                HandleWaitCheckStrtApplStsTimeout();
                break;
            }
            case kWaitVFCResponse:
                m_VFCFlag = false;
            case kWaitOperateEngineResponse:
            case kWaitGetVehicleState:{
                DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "RMT_Engine::GlyTimeout, VFC Start timeout occured");
                SetState(kDone);
                SendError(serviceResult);
                break;
            }
            case kDone:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,"RMT_Engine::GlyTimeout, current state: %s, Nothing to handle",
                                        __FUNCTION__, GetRMT_EngineState().c_str());
                SetState(kDone);
                SendError(serviceResult);
                break;
            }
            default:{
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,"RMT_Engine::GlyTimeout,  UNKNOW timeoutSignal, current state: %s",
                                    __FUNCTION__, GetRMT_EngineState().c_str());
                break;
            }
        }
        return true;
    }

    bool RMT_Engine::AddSignalDeque(std::shared_ptr<fsm::Signal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);

        if(IsSignalDequeActive()){
            std::lock_guard<std::mutex> lock(m_signal_mutex);
            m_signalDeque.push_front(signal);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, deque.size: %d!", __FUNCTION__, m_signalDeque.size());
            if((kNew == GetState()) || (kDone == GetState())){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, notify_one thread", __FUNCTION__);
                m_signal_condition_variable.notify_one();
            }
            return true;
        }
        return false;
    }

    void RMT_Engine::Start(SendSingalCallBack cb)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);
        m_cb = cb;
        m_signal_thread = std::thread(&RMT_Engine::HandleSignalDequeLoop, this);
    }

    bool RMT_Engine::CreateThreadSuccessFul()
    {
        while(!m_create_thread_success){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return m_create_thread_success;
    }

    void RMT_Engine::SendError(ErsResponse_t& serviceResult)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, %s.", __FUNCTION__);

        if (vc::ErsCmd_ErsCmdNotSet != m_operateCmd){
            switch (m_engine_mode){
                case StartEngineMode_APPS:{
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, %s, APP mode", __FUNCTION__);
                    std::shared_ptr<volvo_on_call::AppEngineStatResponseSignal> _res_signal = 
                                  std::make_shared<volvo_on_call::AppEngineStatResponseSignal>();
                    if(nullptr == _res_signal.get()){
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, AppEngineStatResponseSignal is nullptr", __FUNCTION__);
                        return;
                    }
                    _res_signal->SetOperateStatus(false);
                    _res_signal->SetErrorCode(serviceResult.errorCode);
                    _res_signal->SetVehicleErrorCode(serviceResult.vehicleErrorCode);
                    m_cb(_res_signal);
                    break;
                }
                case StartEngineMode_TSP:{
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, %s, TSP mode", __FUNCTION__);
                    RemoveTimeout(m_TSPTimeoutId);
                    //std::shared_ptr<vc::ResRMTStatus> _pResRMTStatus = nullptr;
                    serviceResult.type_ = ErsResponseType_Error;
                    m_pGlyVdsResSignal->SetResultPayload(serviceResult);
                    fsm::VocmoSignalSource vocmo_signal_source;
                    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                                "RMT_Engine::SendError, vocmo_signal_source construct");
                    if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsResSignal))
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, message sent ok.");
                    } else {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMT_Engine::SendError "
                                "Failed to send error message.");
                    }
                    break;
                }
                default:
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, %s, UNKNOW Engine mode", __FUNCTION__);
                    break;
            }
            SetState(kDone);
        }

        if(m_VFCFlag){
            Request_VFCActive(false);
        }

        m_operateCmd = vc::ErsCmd_ErsCmdNotSet;
        NotifyHandleGeelyRequest();
    }

    void RMT_Engine::SendSuccessful(std::shared_ptr<vc::ResRMTStatus> _pResRMTStatus)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s send start successful-response to TSP", __FUNCTION__);
        switch (m_engine_mode){
            case StartEngineMode_APPS:
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, %s, APP mode", __FUNCTION__);
                std::shared_ptr<volvo_on_call::AppEngineStatResponseSignal> _res_signal = 
                                std::make_shared<volvo_on_call::AppEngineStatResponseSignal>();
                if(nullptr == _res_signal.get()){
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, AppEngineStatResponseSignal is nullptr", __FUNCTION__);
                    return;
                }
                _res_signal->SetOperateStatus(true);
                m_cb(_res_signal);
                break;
            }
            case StartEngineMode_TSP:
            {
                RemoveTimeout(m_TSPTimeoutId);
                ErsResponse_t serviceResult;
                serviceResult.operationSucceeded = true;
                serviceResult.type_ = ErsResponseType_Start;
                serviceResult.rmt_status.vehSpdIndcd = _pResRMTStatus->vehSpdIndcd;
                serviceResult.rmt_status.vehSpdIndcdQly = _pResRMTStatus->vehSpdIndcdQly;
                serviceResult.rmt_status.doorPassLockSts = _pResRMTStatus->doorPassLockSts;
                serviceResult.rmt_status.doorPassSts = _pResRMTStatus->doorPassSts;
                serviceResult.rmt_status.doorLeReLockSrs = _pResRMTStatus->doorLeReLockSrs;
                serviceResult.rmt_status.doorLeReSts = _pResRMTStatus->doorLeReSts;
                serviceResult.rmt_status.doorDrvrSts = _pResRMTStatus->doorDrvrSts;
                serviceResult.rmt_status.doorDrvrLockSts = _pResRMTStatus->doorDrvrLockSts;
                serviceResult.rmt_status.doorRiReLockSts = _pResRMTStatus->doorRiReLockSts;
                serviceResult.rmt_status.doorRiReSts = _pResRMTStatus->doorRiReSts;
                serviceResult.rmt_status.lockgCenSts = _pResRMTStatus->lockgCenSts;
                serviceResult.rmt_status.trLockSts = _pResRMTStatus->trLockSts;
                serviceResult.rmt_status.trSts = _pResRMTStatus->trSts;
                serviceResult.rmt_status.hoodSts = _pResRMTStatus->hoodSts;
                serviceResult.rmt_status.lockgCenStsForUsrFb = _pResRMTStatus->lockgCenStsForUsrFb;
                serviceResult.rmt_status.engOilLvlSts = _pResRMTStatus->engOilLvlSts;
                serviceResult.rmt_status.epbSts = _pResRMTStatus->epbSts;
                serviceResult.rmt_status.fuLvlIndcd = _pResRMTStatus->fuLvlIndcd;
                serviceResult.rmt_status.fuLvlLoIndcn = _pResRMTStatus->fuLvlLoIndcn;
                serviceResult.rmt_status.fulLvlLoWarn = _pResRMTStatus->fulLvlLoWarn;
                serviceResult.rmt_status.sunroofPosnSts = _pResRMTStatus->sunroofPosnSts;
                serviceResult.rmt_status.winPosnStsAtDrv = _pResRMTStatus->winPosnStsAtDrv;
                serviceResult.rmt_status.winPosnStsAtPass = _pResRMTStatus->winPosnStsAtPass;
                serviceResult.rmt_status.winPosnStsReLe = _pResRMTStatus->winPosnStsReLe;
                serviceResult.rmt_status.winPosnStsReRi = _pResRMTStatus->winPosnStsReRi;
                serviceResult.rmt_status.ventilateStatus = _pResRMTStatus->ventilateStatus;
                serviceResult.rmt_status.interiorPM25Lvl = _pResRMTStatus->interiorPM25Lvl;
                serviceResult.rmt_status.exteriorPM25Lvl = _pResRMTStatus->exteriorPM25Lvl;
                m_pGlyVdsResSignal->SetResultPayload(serviceResult);
                fsm::VocmoSignalSource vocmo_signal_source;
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                        " RMT_Engine::%s, vocmo_signal_source construct", __FUNCTION__);
                if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsResSignal))
                {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_Engine::%s, message sent ok.", __FUNCTION__);
                } else {
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_Engine::%s "
                        "Failed to send error message.", __FUNCTION__);
                }
                    break;
                }
            case StartEngineMode_No:
            default:
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::SendError, %s, UNKNOW Engine mode", __FUNCTION__);
                break;
            }
        }
        m_operateCmd = vc::ErsCmd_ErsCmdNotSet;
        SetState(kDone);

        if(m_VFCFlag){
            Request_VFCActive(false);
        }

        NotifyHandleGeelyRequest();
    }

    void RMT_Engine::SendStopSuccessful(vc::CarUsageModeState state)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s send stop successful-response to TSP", __FUNCTION__);
        RemoveTimeout(m_TSPTimeoutId);

        ErsResponse_t serviceResult;
        serviceResult.operationSucceeded = true;
        serviceResult.type_ = ErsResponseType_Stop;
        serviceResult.usgeMode_state = state;
        m_pGlyVdsResSignal->SetResultPayload(serviceResult);
        fsm::VocmoSignalSource vocmo_signal_source;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                " RMT_Engine::%s, vocmo_signal_source construct", __FUNCTION__);
        if (vocmo_signal_source.GeelySendMessage(m_pGlyVdsResSignal))
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, " RMT_Engine::%s, message sent ok.", __FUNCTION__);
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMT_Engine::%s "
                "Failed to send error message.", __FUNCTION__);
        }
        m_operateCmd = vc::ErsCmd_ErsCmdNotSet;
        SetState(kDone);

        if(m_VFCFlag){
            Request_VFCActive(false);
        }

        NotifyHandleGeelyRequest();
    }

    void RMT_Engine::HandleSignalDequeLoop()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);
        m_create_thread_success = true;
        while(!m_finalizing){
            std::unique_lock<std::mutex> lock(m_signal_mutex);
            m_signal_condition_variable.wait(lock);//, [&]{ return ((m_signalDeque.size() != 0) || m_finalizing); });
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, handle signal", __FUNCTION__);
            std::shared_ptr<fsm::Signal> signal = m_signalDeque.front();
            m_signalDeque.pop_front();
            m_pGlyVdsResSignal = std::dynamic_pointer_cast<volvo_on_call::GlyVdsResSignal>(signal);
            if(!HandleGlyRequest(signal)){
                m_finalizing = true;
            }
        }
    }

    void RMT_Engine::StopLoop()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);
        m_create_thread_success = false;
        if(IsSignalDequeActive()){
            m_finalizing = true;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, notify thread", __FUNCTION__);
            m_signal_condition_variable.notify_one();
        }
    }

    void RMT_Engine::TerminateSignalThread()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s", __FUNCTION__);
        if(IsSignalDequeActive())
            StopLoop();
        if(m_signal_thread.joinable())
            m_signal_thread.join();
    }

    void RMT_Engine::NotifyHandleGeelyRequest()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, deque.size: %d!", __FUNCTION__, m_signalDeque.size());
        m_close_flag = true;
        if(0 < m_signalDeque.size()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, notify thread", __FUNCTION__);
            m_signal_condition_variable.notify_one();
        }
    }

    void RMT_Engine::HandleWaitCheckStrtApplStsTimeout()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s ", __FUNCTION__);
        ErsResponse_t serviceResult;
        serviceResult.operationSucceeded = false;
        serviceResult.errorCode = executionFailure;
        switch (m_operateCmd)
        {
            case vc::ErsCmd_ErsCmdOn:
                {
                    if(vc::ErsStrtApplSts_ErsStsRunng == m_ersStrtAppStsInt || vc::ErsStrtApplSts_ErsStsStrtg == m_ersStrtAppStsInt){
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, has been start success for request", __FUNCTION__);
                        SetState(kDone);
                        serviceResult.message = RMT_ERS_STARTING_OR_RUNNING;
                        SendError(serviceResult);
                    } else {
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, start request", __FUNCTION__);
                        Request_OperateEngine();
                    }
                }
                break;
            case vc::ErsCmd_ErsCmdOff:
                {
                    if(vc::ErsStrtApplSts_ErsStsOff == m_ersStrtAppStsInt){
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, has been stop success for request", __FUNCTION__);
                        SetState(kDone);
                        serviceResult.message = RMT_ERS_STOPED;
                        SendError(serviceResult);
                    } else{
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "RMT_Engine::%s, stop request", __FUNCTION__);
                        Request_OperateEngine();
                    }
                }
                break;
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, Nothing to do ", __FUNCTION__);
                break;
        }
    }

    bool RMT_Engine::IsCurrentRequestVaild()
    {
        if(!m_close_flag){//current request is unvaild
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, current reqeust is unvalid! ", __FUNCTION__);
            SetState(kDone);
            NotifyHandleGeelyRequest();
        }
        return m_close_flag;
    }

    bool RMT_Engine::Handle_WaitOperateEngineResponseState(std::shared_ptr<fsm::RMTEngineSignal> signal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s ", __FUNCTION__);
        if (vc::ErsStrtRes_ErsStrtSuccess == signal->GetData()->ersStrtRes)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, state: %s", __FUNCTION__, GetRMT_EngineState().c_str());
            switch (m_operateCmd)
            {
                case vc::ErsCmd_ErsCmdOn:{
                     DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, ErsCmd_ErsCmdOn, state: %s",
                                            __FUNCTION__, GetRMT_EngineState().c_str());
                    if(StartEngineMode_APPS == m_engine_mode)
                    { // engine is started by APP
                        RemoveTimeout(m_timeoutTransactionId);
                        std::shared_ptr<volvo_on_call::AppEngineStatResponseSignal> _res_signal =
                                std::make_shared<volvo_on_call::AppEngineStatResponseSignal>();
                        if(nullptr == _res_signal.get()){
                            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, create AppEngineStatResponseSignal error", __FUNCTION__);
                            return false;
                        }
                        _res_signal->SetOperateStatus(true);
                        _res_signal->SetErsStartApplSts(vc::ErsStrtApplSts_ErsStsRunng);
                        _res_signal->SetEngStlWdSts(vc::EngSt1_RunngRemStrtd);
                        m_cb(_res_signal);
                        return true;
                    } else if(StartEngineMode_TSP == m_engine_mode){
                        if((vc::ErsStrtApplSts_ErsStsRunng == signal->GetData()->ersStrtApplSts)
                            && (vc::ErsCmd_ErsCmdOn == m_operateCmd))
                        {// Engine running
                            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, Engine Running Now", __FUNCTION__);
                            //SetState(kWaitGetVehicleState);
                            Request_VehicleStates();
                        } else if ((vc::ErsStrtApplSts_ErsStsStrtg == signal->GetData()->ersStrtApplSts)
                                    && (vc::ErsCmd_ErsCmdOn == m_operateCmd))
                        {// Engine starting
                            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s , start success", __FUNCTION__);
                        }
                    }
                    break;
                }
                case vc::ErsCmd_ErsCmdOff:{
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine::%s, ErsCmd_ErsCmdOff", __FUNCTION__);
                    RemoveTimeout(m_timeoutTransactionId);
                    //TODO, after complete, delete RemoveTimeout() function;
                    Request_VehicleStates();
                    break;
                }
                default:
                    break;
            }
        } else {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMT_Engine::HandleRMTEngineSignal, failed");
            RemoveTimeout(m_timeoutTransactionId);
            ErsResponse_t serviceResult;
            serviceResult.operationSucceeded = false;
            serviceResult.errorCode = executionFailure;
            serviceResult.vehicleErrorCode = signal->GetData()->ersStrtRes;
            SendError(serviceResult);
            SetState(kDone);
        }
        return true;
    }

    std::string RMT_Engine::GetRMT_EngineState()
    {
        States state = (States)GetState();
        ITER it = states_map.find(state);
        if (it == states_map.end()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMT_Engine:: %s, UNKNOWState ", __FUNCTION__);
            return "UNKnowState";
        } else {
            return it->second;
        }
    }
}
// \} end of addtogroup
