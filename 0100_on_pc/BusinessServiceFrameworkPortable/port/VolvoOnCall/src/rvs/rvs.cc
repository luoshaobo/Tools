/**
 * Copyright (C) 2018-2019 Continental Automotive IC ShangHai
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     rvs_transaction.cc
 *  \brief    Rvs transaction.
 *  \author   Jun Yang
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "dlt/dlt.h"
#include "rvs/rvs.h"
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include "voc_framework/signal_sources/timeout_signal_source.h"
#include "signals/geely/gly_vds_rvs_signal.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/ip_command_broker_signal_source.h"
#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"

#include "remote_config_persist_interface.hpp"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

#define STATUS_MAX_RULE  (5)  //Maximum rules can be installed in TEM of RVS service.
//#define STATUS_UPLOAD_INTERVAL (600)   //Upload interval of RVS service.
#define STATUS_UPLOAD_INTERVAL (60)   //Upload interval of RVS service.
#define STATUS_UPLOAD_MAX (800)  //Maximum number of upload status of signals of RVS service.

//Change from 10 to 1 min per new REQ
#define RVS_INTERVAL (1) //Vehicle status report interval mainly for PM 2.5 and driving behavior, in minutes

#define STATUS_COLLECT_INTERVAL (10)
#define IPCOMMAND_RESPONSE_TIMEOUT (10)
#define VPOM_POSITION_RESPONSE_TIMEOUT (10)

typedef enum RvsServiceCommand {
    startService = 0,
    requestData = 1,
    responseData = 2,
    executionAction = 4,
    terminateService = 20
} e_RvsServiceCommand;

Rvs::Rvs() :
fsm::SmartTransaction(kRvsStart)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS1]%s", __FUNCTION__);
    SignalFunction handle_rvs_request_enqueue =
        std::bind(&Rvs::HandleRvsRequestEnqueue,
        this,
        std::placeholders::_1);

    SignalFunction handle_rvs_new_request =
        std::bind(&Rvs::HandleRvsNewRequest,
        this,
        std::placeholders::_1);

    SignalFunction handle_rvs_process_request =
        std::bind(&Rvs::HandleRvsProcessRequest,
        this,
        std::placeholders::_1);

    SignalFunction handle_rvs_collect_vehicle_state =
        std::bind(&Rvs::HandleRvsCollectVehicleState,
        this,
        std::placeholders::_1);

    SignalFunction handle_rvs_receive_vehicle_state =
        std::bind(&Rvs::HandleRvsReceiveVehicleState,
        this,
        std::placeholders::_1);

    SignalFunction handle_rvs_upload_vehicle_state =
        std::bind(&Rvs::HandleRvsUploadVehicleState,
        this,
        std::placeholders::_1);

    SignalFunction handle_rvs_timeout =
        std::bind(&Rvs::HandleRvsTimeout,
        this,
        std::placeholders::_1);

    SignalFunction handle_position =
        std::bind(&Rvs::HandlePosition,
        this,
        std::placeholders::_1);

    SignalFunction handle_carmode_signal =
        std::bind(&Rvs::HandleCarModeSignal,
        this,
        std::placeholders::_1);

    SignalFunction handle_vin_signal =
        std::bind(&Rvs::HandleVinNumberSignal,
        this,
        std::placeholders::_1);


    StateFunction EnterRvsStartState =
        std::bind(&Rvs::EnterRvsStartState,
        this,
        std::placeholders::_1,
        std::placeholders::_2);

    StateFunction EnterRvsExecuteState =
        std::bind(&Rvs::EnterRvsExecuteState,
        this,
        std::placeholders::_1,
        std::placeholders::_2);

    StateFunction EnterRvsConfigState =
        std::bind(&Rvs::EnterRvsConfigState,
        this,
        std::placeholders::_1,
        std::placeholders::_2);

    StateFunction EnterRvsIdleState =
        std::bind(&Rvs::EnterRvsIdleState,
        this,
        std::placeholders::_1,
        std::placeholders::_2);

    StateFunction EnterRvsUploadState =
        std::bind(&Rvs::EnterRvsUploadState,
        this,
        std::placeholders::_1,
        std::placeholders::_2);

    StateFunction EnterRvsStopState =
        std::bind(&Rvs::EnterRvsStopState,
        this,
        std::placeholders::_1,
        std::placeholders::_2);


    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS2]%s", __FUNCTION__);

    StateMap state_map =
        //{{state,                     {state_handler,
        //                                {{signal_type, signal_handler}},
        //                                {legal states to transition to}}}}
    {{kRvsStart,            {EnterRvsStartState,
                                {{fsm::Signal::kTimeout, handle_rvs_timeout},
                                {fsm::kVDServiceRvs, handle_rvs_request_enqueue}},
                                {kRvsIdle, kRvsConfig, kRvsExecute, kRvsUpload, kRvsStop}}},
    {kRvsIdle,                  {EnterRvsIdleState,
                                {{fsm::Signal::kTimeout, handle_rvs_timeout},
                                {fsm::kVDServiceRvs, handle_rvs_request_enqueue},
                                {kSignal_NewRequest, handle_rvs_new_request},
                                {kSignal_ProcessRequest, handle_rvs_process_request},
                                {kSignal_CollectVehicleState, handle_rvs_collect_vehicle_state},
                                {kSignal_ReceiveVehicleState, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kTelmDshb, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckResponse, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckNotify, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                                {fsm::Signal::kCarMode, handle_carmode_signal},
                                {fsm::Signal::kCarUsageMode, handle_carmode_signal},
                                {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                                {kSignal_UploadVehicleState, handle_rvs_upload_vehicle_state}},
                                {kRvsConfig, kRvsExecute, kRvsUpload, kRvsStop}}},
    {kRvsConfig,           {EnterRvsConfigState,
                                {{fsm::Signal::kTimeout, handle_rvs_timeout},
                                {fsm::kVDServiceRvs, handle_rvs_request_enqueue},
                                {kSignal_ProcessRequest, handle_rvs_process_request},
                                {kSignal_CollectVehicleState, handle_rvs_collect_vehicle_state},
                                {kSignal_ReceiveVehicleState, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kTelmDshb, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckResponse, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckNotify, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                                {fsm::Signal::kCarMode, handle_carmode_signal},
                                {fsm::Signal::kCarUsageMode, handle_carmode_signal},
                                {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                                {kSignal_UploadVehicleState, handle_rvs_upload_vehicle_state}},
                                {kRvsIdle, kRvsExecute, kRvsUpload, kRvsStop}}},
    {kRvsExecute,           {EnterRvsExecuteState,
                                {{fsm::Signal::kTimeout, handle_rvs_timeout},
                                {fsm::kVDServiceRvs, handle_rvs_request_enqueue},
                                {kSignal_ProcessRequest, handle_rvs_process_request},
                                {kSignal_CollectVehicleState, handle_rvs_collect_vehicle_state},
                                {kSignal_ReceiveVehicleState, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kTelmDshb, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckResponse, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckNotify, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                                {fsm::Signal::kCarMode, handle_carmode_signal},
                                {fsm::Signal::kCarUsageMode, handle_carmode_signal},
                                {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                                {kSignal_UploadVehicleState, handle_rvs_upload_vehicle_state}},
                                {kRvsIdle, kRvsConfig, kRvsUpload, kRvsStop}}},
    {kRvsUpload,           {EnterRvsUploadState,
                                {{fsm::Signal::kTimeout, handle_rvs_timeout},
                                {fsm::kVDServiceRvs, handle_rvs_request_enqueue},
                                {kSignal_ProcessRequest, handle_rvs_process_request},
                                {kSignal_CollectVehicleState, handle_rvs_collect_vehicle_state},
                                {kSignal_ReceiveVehicleState, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kTelmDshb, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckResponse, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kVehSoHCheckNotify, handle_rvs_receive_vehicle_state},
                                {fsm::Signal::kGNSSPositionDataSignal, handle_position},
                                {fsm::Signal::kCarMode, handle_carmode_signal},
                                {fsm::Signal::kCarUsageMode, handle_carmode_signal},
                                {fsm::Signal::kVINNumberSignal, handle_vin_signal},
                                {kSignal_UploadVehicleState, handle_rvs_upload_vehicle_state}},
                                {kRvsIdle, kRvsConfig, kRvsExecute, kRvsStop}}},
    {kRvsStop,              {EnterRvsStopState,
                                SignalFunctionMap(),
                                {}}}};

    SetStateMap(state_map);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS3]%s", __FUNCTION__);

    MapSignalType(kSignal_ProcessRequest);
    MapSignalType(kSignal_CollectVehicleState);
    MapSignalType(kSignal_ReceiveVehicleState);
    MapSignalType(kSignal_UploadVehicleState);
    MapSignalType(fsm::Signal::kTelmDshb);
    MapSignalType(fsm::Signal::kVehSoHCheckResponse);
    MapSignalType(fsm::Signal::kVehSoHCheckNotify);
    MapSignalType(fsm::Signal::kGNSSPositionDataSignal);
    MapSignalType(fsm::Signal::kCarMode);
    MapSignalType(fsm::Signal::kCarUsageMode);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS4]%s", __FUNCTION__);

    //m_uploadInterval = STATUS_UPLOAD_INTERVAL;
    UpdateConfiguration();
    m_collectInterval = STATUS_COLLECT_INTERVAL;
    old_srvTrig = 0;
    m_needUploadTimes = 0;

    m_configType = kRvs_all_config;

    m_triggerType = kTriggerType_Unknown;

    InitializationVechicleStatus();

    SetState(kRvsIdle);

    // Initialeze signal queue
    //lijing-test
    #if 1
    fsm::VdServiceTransactionId vdsTransId;
    VDServiceRequest *vdmsg = nullptr;
    std::shared_ptr<fsm::Signal> signal = GlyVdsRvsSignal::CreateGlyVdsRvsSignal(vdsTransId, vdmsg);

    if (signal.get() == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS] CreateGlyVdsRvsSignal == nullptr", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, call HandleRvsRequestEnqueue, signal addr=%0x",
                        __FUNCTION__,
                        signal.get());
        HandleRvsRequestEnqueue(signal);
    }
    #endif

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, call RequestCarUsageMode",
                    __FUNCTION__);
    RequestCarUsageMode();
    RequestVinNumber();
    
}

Rvs::~Rvs()
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,"Rvs::~Rvs\n");
}

void Rvs::InitializationVechicleStatus()
{
    memset(&m_bs, 0x0, sizeof(Rvs_BS_t));
    memset(&m_bs.position, 0x0, sizeof(RvsPosition_t));

    memset(&m_as, 0x0, sizeof(Rvs_AS_t));
    memset(&m_as.confAndId, 0x0, sizeof(Rvs_AS_CI_t));
    memset(&m_as.drivingSafetyStatus, 0x0, sizeof(Rvs_AS_DS_t));
    memset(&m_as.maintenanceStatus, 0x0, sizeof(Rvs_AS_MS_t));
    memset(&m_as.maintenanceStatus.mainBatteryStatus, 0x0, sizeof(RvsBatteryStatus_t));
    memset(&m_as.runningStatus, 0x0, sizeof(Rvs_AS_RS_t));
    memset(&m_as.climateStatus, 0x0, sizeof(Rvs_AS_CS_t));
    memset(&m_as.drivingBehaviourStatus, 0x0, sizeof(Rvs_AS_DB_t));
    memset(&m_as.pollutionStatus, 0x0, sizeof(Rvs_AS_PS_t));
    memset(&m_as.ecuWarningMessages, 0x0, sizeof(Rvs_AS_CC_t));
    memset(&m_as.electricVehicleStatus, 0x0, sizeof(Rvs_AS_ES_t));
    memset(&m_as.carConfig, 0x0, sizeof(Rvs_AS_CC_t));
    memset(&m_as.signals, 0x0, sizeof(Rvs_AS_SIGNALS_KEY_t));
    memset(&m_as.data, 0x0, 32*sizeof(uint8_t));

    memset(&m_ts, 0x0, sizeof(Rvs_TS_t));
    memset(&m_ts.vin, 0x0, RVS_VIN_BUF_SIZE*sizeof(uint8_t));
    memset(&m_ts.backupBattery, 0x0, sizeof(RvsBatteryStatus_t));
    memset(&m_ts.sleepCycleNextWakeupTime, 0x0, sizeof(RvsTimeStamp_t));
    memset(&m_ts.diagnostics, 0x0, sizeof(RvsECUDiagnostic_t));
    memset(&m_ts.networkAccessStatus, 0x0, sizeof(RvsNetworkAccessStatus_t));
    memset(&m_ts.serialNumber, 0x0, 32*sizeof(uint8_t));
    memset(&m_ts.completeECUPartNumbers, 0x0, 32*sizeof(uint8_t));
    memset(&m_ts.imei, 0x0, 32*sizeof(uint8_t));
    memset(&m_ts.hwVersion, 0x0, 32*sizeof(uint8_t));
    memset(&m_ts.swVersion, 0x0, 32*sizeof(uint8_t));
    memset(&m_ts.mpuVersion, 0x0, 32*sizeof(uint8_t));
    memset(&m_ts.mcuVersion, 0x0, 32*sizeof(uint8_t));
}

std::string Rvs::StateToString(State state)
{
    std::string return_value = "UnknowState";
    switch(state)
    {
    case kRvsStart:
        return_value =  "kRvsStart";
        break;
    case kRvsIdle:
        return_value =  "kRvsIdle";
        break;
    case kRvsConfig:
        return_value =  "kRvsConfig";
        break;
    case kRvsExecute:
        return_value =  "kRvsExecute";
        break;
    case kRvsUpload:
        return_value =  "kRvsUpload";
        break;
    case kRvsStop:
        return_value =  "kRvsStop";
        break;
    default:
        break;
    }

    return return_value;
}

void Rvs::StartTimer(fsm::TimeoutTransactionId timerTickId, long timeInterval)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]StartTimer");
    RemoveTimeout(timerTickId);
    timerTickId = RequestTimeout(static_cast<std::chrono::seconds>(timeInterval), false);
    MapTransactionId(timerTickId.GetSharedCopy());
}

void Rvs::StopTimer(fsm::TimeoutTransactionId timerTickId)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]StopTimer");
    RemoveTimeout(timerTickId);
}


/************************************************************/
// @brief         Callback function to HandleRvsTimeout
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsTimeout(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    if(signal->GetTransactionId() == m_kRvs_Ipcommand_tick_id)
    {
        StopTimer(m_kRvs_Ipcommand_tick_id);
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS] m_kRvs_Ipcommand_tick_id, timeout!!!!!!!!!\n");
    }

    if(signal->GetTransactionId() == m_kRvs_period_tick_id)
    {
//        StopTimer(m_kRvs_period_tick_id);
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS] m_kRvs_period_tick_id, timeout!!!!!!!!!\n");

        SetState(kRvsExecute);
        std::shared_ptr<fsm::Signal> RvsSignalCollect = std::make_shared<RvsSignalCollectVehicleState>();
        EnqueueSignal(RvsSignalCollect);

        m_needUploadTimes++;
    }

    if(signal->GetTransactionId() == m_kRvs_request_position_tick_id)
    {
        StopTimer(m_kRvs_request_position_tick_id);
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS] m_kRvs_request_position_tick_id, timeout!!!!!!!!!\n");
    }
    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsRequestEnqueue
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsRequestEnqueue(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;
    bool ret = false;
    RvsNewSignal_t rvsNewSignalIn;
    RvsNewSignal_t rvsNewSignalOut;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s signal addr:%0x\n", __FUNCTION__,signal.get());

    rvsNewSignalIn.signal = signal;
    rvsNewSignalIn.status = 1;
    m_SignalRequestList.push_back(rvsNewSignalIn);

    //if state in Idle, need trigger one signal.
    if(GetState() == kRvsIdle)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, State in Idle, need trigger one signal!!!", __FUNCTION__);
        ret = HandleRvsRequestDequeue(&rvsNewSignalOut);
        if(true == ret)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "[RVS]%s, call HandleRvsNewRequest; ", __FUNCTION__);
            HandleRvsNewRequest(rvsNewSignalOut.signal);
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                    "[RVS]%s, state is not kRvsIdle, not dequee and process it, state = %s",
                    __FUNCTION__, StateToString(GetState()).c_str());
    }
    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsRequestDequeue
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsRequestDequeue(RvsNewSignal_t *signal)
{
    bool return_value = false;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    if(!m_SignalRequestList.empty())
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, m_SignalRequestList.size:%d\n", __FUNCTION__,m_SignalRequestList.size());
        *signal = m_SignalRequestList.front();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s signal addr:%0x\n", __FUNCTION__,signal->signal.get());
        m_SignalRequestList.pop_front();
        return_value = true;
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                "[RVS]%s, m_SignalRequestList is empty; \n",
                __FUNCTION__);
    }

    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsNewRequest
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsNewRequest(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    m_newRvsSignal = std::dynamic_pointer_cast<GlyVdsRvsSignal>(signal);
    if(m_newRvsSignal != nullptr)
    {
        //to parse request payload, store config
        //after store config, to process request, enter Execute, collect Vehicle state
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s() lijing-test, set state to %s \n ",
                __FUNCTION__, StateToString(kRvsConfig).c_str());
        SetState(kRvsConfig);
        std::shared_ptr<fsm::Signal> RvsSignalProcess = std::make_shared<RvsSignalProcessRequest>();
        EnqueueSignal(RvsSignalProcess);
    }
    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsProcessRequest
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsProcessRequest(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;

    std::shared_ptr<RvsRequest_t> requestPayload = m_newRvsSignal->GetRequestPayload();
    //lijing-test
    if(requestPayload == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, requestPayload is null",
                __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, requestPayload is not null",
                __FUNCTION__);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, requestPayload->serviceId %d", __FUNCTION__, requestPayload->ServiceId);

    //ServiceCommand is null , this is one shot or Multiple shot periodically
    if(requestPayload->ServiceCommandIsNull == true)
    {
        if(requestPayload->ServiceParamCount == 0) // one shot
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, one shot!!!!", __FUNCTION__);
            m_configType = kRvs_all_config;
        }
        else//multiple shot periodically
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, multiple shot periodically!!!!", __FUNCTION__);
            for(int i =0; i <requestPayload->ServiceParamCount; i++)
            {
                if (strncmp("collectInterval", requestPayload->ServiceParam[i].Key, requestPayload->ServiceParam[i].keysize) == 0)
                {

                }
                else if (strncmp("uploadInterval", requestPayload->ServiceParam[i].Key, requestPayload->ServiceParam[i].keysize) == 0)
                {
                    m_uploadInterval = requestPayload->ServiceParam[i].IntVal;
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, uploadInterval, m_uploadInterval = %d!!!!", __FUNCTION__, m_uploadInterval);
                    if (m_uploadInterval > 0)
                    {
                        UpdateTriggerType(kTriggerChgRsn_TimerChg);
                    }
                }
                else if (strncmp("startTime", requestPayload->ServiceParam[i].Key, requestPayload->ServiceParam[i].keysize) == 0)
                {

                }
                else if (strncmp("endTime", requestPayload->ServiceParam[i].Key, requestPayload->ServiceParam[i].keysize) == 0)
                {

                }
                else if (strncmp("signalName", requestPayload->ServiceParam[i].Key, requestPayload->ServiceParam[i].keysize) == 0)
                {
                    UpdateConfigType(requestPayload->ServiceParam[i].StringVal);
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, UpdateConfigType, m_configType = %d!!!!", __FUNCTION__, m_configType);
                }
            }
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, event trigger!!!!", __FUNCTION__);
        //ServiceCommand is not null,  this is event trigger
        if(requestPayload->ServiceCommand == startService)
        {
            SendVehSoHCheckRequest();
        }
        else if(requestPayload->ServiceCommand == terminateService)
        {

        }
    }

    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsCollectVehicleState
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsCollectVehicleState(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    // start Timer, for get Ipcommand timeout
    RemoveTimeout(m_kRvs_Ipcommand_tick_id);
    m_kRvs_Ipcommand_tick_id = RequestTimeout(static_cast<std::chrono::seconds>(IPCOMMAND_RESPONSE_TIMEOUT), false);
    MapTransactionId(m_kRvs_Ipcommand_tick_id.GetSharedCopy());

    // request Position
    RequestPosition(signal->GetTransactionId().GetSharedCopy());

    SetState(kRvsUpload);
    std::shared_ptr<fsm::Signal> RvsSignalReceive = std::make_shared<RvsSignalReceiveVehicleState>();
    EnqueueSignal(RvsSignalReceive);

    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsReceiveVehicleState
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsReceiveVehicleState(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, m_needUploadTimes = %d", __FUNCTION__, m_needUploadTimes);

    StopTimer(m_kRvs_Ipcommand_tick_id);

    if(signal->GetSignalType() == fsm::Signal::kTelmDshb)
    {
        return_value = ReceiveTelmDshbData(signal);
    }

    if(signal->GetSignalType() == fsm::Signal::kVehSoHCheckResponse)
    {
        return_value = ReceiveVehSoHCheckData(signal);
    }

    if(signal->GetSignalType() == fsm::Signal::kVehSoHCheckNotify)
    {
        return_value = ReceiveVehSoHCheckData(signal);
    }

    if(m_needUploadTimes > 0)
    {
        std::shared_ptr<fsm::Signal> RvsSignalUpload = std::make_shared<RvsSignalUploadVehicleState>();
        EnqueueSignal(RvsSignalUpload);
        m_needUploadTimes--;
    }

    return return_value;
}

/************************************************************/
// @brief         Callback function to HandleRvsUploadVehicleState
// @param[in]  fsm::Signal
// @return       true or false
/************************************************************/
bool Rvs::HandleRvsUploadVehicleState(std::shared_ptr<fsm::Signal> signal)
{
    bool ret = false;
    bool return_value = true;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    std::shared_ptr<RvsRequest_t> requestPayload = m_newRvsSignal->GetRequestPayload();
    requestPayload->ServiceId = 51;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "rvs requestPayload->serviceId %d", requestPayload->ServiceId);

#if 0
    Rvs_TS_t  ts = {0};
    ComposeRVSSimuData(ts);

    Rvs_AS_DS_t as_ds = {0};
    ComposeRVSSimuData(as_ds);

    Rvs_AS_CI_t as_ci = {0};
    memcpy(as_ci.vin, "abcdefg", 7);
    as_ci.fuelType = 111;


    Rvs_AS_MS_t as_ms = {0};
    ComposeRVSSimuData(as_ms);

    Rvs_AS_RS_t as_rs = {0};
    ComposeRVSSimuData(as_rs);


    Rvs_AS_CS_t as_cs = {0};
    ComposeRVSSimuData(as_cs);


    Rvs_AS_DB_t as_db = {0};
    ComposeRVSSimuData(as_db);

    Rvs_AS_ES_t as_es = {0};
    ComposeRVSSimuData(as_es);
#endif

    m_newRvsSignal->SetHeadPayload();
    m_newRvsSignal->SetBodyPayload();

    ret = CheckWhetherExistConfigType(m_configType, kRvs_bs);
    if(true == ret)
    {
        m_newRvsSignal->SetBasicVehicleStatusPayload(m_basicVS);
    }

    ret = CheckWhetherExistConfigType(m_configType, kRvs_ls);
    if(true == ret)
    {
        //m_newRvsSignal->SetBsPayload(m_bs);
        m_newRvsSignal->SetLockStatusPayload(m_ls);
    }

    ret = CheckWhetherExistConfigType(m_configType, kRvs_ms);
    if(true == ret)
    {
        //m_newRvsSignal->SetBsPayload(m_bs);
        m_newRvsSignal->SetMaintenanceStatusPayload(m_ms);
    }

    ret = CheckWhetherExistConfigType(m_configType, kRvs_rs);
    if(true == ret)
    {
        //m_newRvsSignal->SetBsPayload(m_bs);
        m_newRvsSignal->SetRunningStatusPayload(m_rs);
    }

    ret = CheckWhetherExistConfigType(m_configType, kRvs_cs);
    if(true == ret)
    {
        //m_newRvsSignal->SetBsPayload(m_bs);
        m_newRvsSignal->SetClimateStatusPayload(m_cs);
    }

    ret = CheckWhetherExistConfigType(m_configType, kRvs_es);
    if(true == ret)
    {
        m_newRvsSignal->SetElectricStatusPayload(m_es);
    }

    ret = CheckWhetherExistConfigType(m_configType, kRvs_ps);
    if(true == ret)
    {
        m_newRvsSignal->SetPollutionStatusPayload(m_ps);
    }


    ret = CheckWhetherExistConfigType(m_configType, kRvs_as);
    if(true == ret)
    {
        //Not implemented.
        //m_newRvsSignal->SetAdditionalStatusPayload(m_AdditionalStatus);
    }
#if 0
    else
    {
        ret = CheckWhetherExistConfigType(m_configType, kRvs_as_ci);
        if(true == ret)
        {
            m_newRvsSignal->SetAsCiPayload(as_ci);
        }

        ret = CheckWhetherExistConfigType(m_configType, kRvs_as_ds);
        if(true == ret)
        {
            //m_newRvsSignal->SetAsDsPayload(as_ds);
            m_newRvsSignal->SetAsDsPayload(m_as.drivingSafetyStatus);
        }

        ret = CheckWhetherExistConfigType(m_configType, kRvs_as_cc);
        if(true == ret)
        {
            m_newRvsSignal->SetAsCcPayload(as_cc);
        }
    }
#endif

    fsm::VocmoSignalSource vocmo_signal_source;

    bool result = vocmo_signal_source.GeelySendMessage(m_newRvsSignal);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]GeelySendMessage->result %d", result);
    result = true;
    SetState(kRvsIdle);
    UpdateTriggerType(kTriggerChgRsn_Uploaded);
    return return_value;
}

bool Rvs::HandlePosition(std::shared_ptr<fsm::Signal> signal)
{
    State state = GetState();
    vpom::GNSSData gnss_position;

    RemoveTimeout(m_kRvs_request_position_tick_id);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s processing,state:%d.\n", __FUNCTION__,static_cast<int>(state));

    if(signal->GetSignalType() != fsm::Signal::kGNSSPositionDataSignal)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s received unexpected signal type:%d.\n", __FUNCTION__);
        return true;
    }

    std::shared_ptr<fsm::VpomGNSSPositionDataSignal> position_signal = std::static_pointer_cast<fsm::VpomGNSSPositionDataSignal>(signal);
    gnss_position = position_signal->GetPayload();
    /*
    if(gnss_position.datastatus == vpom::GNSS_DATA_INVALID)
    {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s vpom::GNSS_DATA_INVALID!!!\n", __FUNCTION__);
    RequestPosition(signal->GetTransactionId().GetSharedCopy());
    }
    else
    {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]HandlePosition-->position.longlat.latitude:%d.\n", gnss_position.position.longlat.latitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]HandlePosition-->position.longlat.longitude:%d.\n", gnss_position.position.longlat.longitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]HandlePosition-->position.altitude:%d.\n", gnss_position.position.altitude);
    m_bs.position.latitude = gnss_position.position.longlat.latitude; // Latitude
    m_bs.position.longitude = gnss_position.position.longlat.longitude; // Longitude
    m_bs.position.altitude = gnss_position.position.altitude; // Altitide from mean sea level
    m_bs.position.posCanBeTrusted = 1;
    m_bs.position.carLocatorStatUploadEn = 1;
    m_bs.position.marsCoordinates = 0;
    }
    */

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]HandlePosition-->position.longlat.latitude:%d.\n", gnss_position.position.longlat.latitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]HandlePosition-->position.longlat.longitude:%d.\n", gnss_position.position.longlat.longitude);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]HandlePosition-->position.altitude:%d.\n", gnss_position.position.altitude);
    m_basicVS.position.latitude = gnss_position.position.longlat.latitude; // Latitude
    m_basicVS.position.longitude = gnss_position.position.longlat.longitude; // Longitude
    m_basicVS.position.altitude = gnss_position.position.altitude; // Altitide from mean sea level
    m_basicVS.position.posCanBeTrusted = 1;
    m_basicVS.position.carLocatorStatUploadEn = 1;
    m_basicVS.position.marsCoordinates = 0;
    return true;
}

/**
* \brief handle fsm::Signal::kCarMode, fsm::Signal::kCarUsageMode
*
* \param[in] received signals
*
* \return True if transaction is not yet finished, false if it is finished.
*/
bool Rvs::HandleCarModeSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    switch (signal->GetSignalType())
    {
    case fsm::Signal::kCarMode:
        {
            // cast to the correct message class:
            std::shared_ptr<fsm::CarModeSignal> car_mode_response =  std::static_pointer_cast<fsm::CarModeSignal>(signal);
            vc::ReturnValue vc_return_value = car_mode_response->GetVcStatus();

            if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "[RVS]%s, Failed to issue request to VehicleComm", __FUNCTION__);
            }
            else
            {
                m_carMod = car_mode_response->GetData()->carmode;

                UpdateTriggerType(kTriggerChgRsn_CarModChg);
            }
            break;
        }
    case fsm::Signal::kCarUsageMode:
        {
            // cast to the correct message class:
            std::shared_ptr<fsm::CarUsageModeSignal> car_usage_mode_response = std::static_pointer_cast<fsm::CarUsageModeSignal>(signal);
            vc::ReturnValue vc_return_value = car_usage_mode_response->GetVcStatus();
            if (vc_return_value != vc::RET_OK)
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "[RVS]%s, Failed to issue request to VehicleComm", __FUNCTION__);
            }
            else
            {
                m_carUsgMod = car_usage_mode_response->GetData()->usagemode;

                UpdateTriggerType(kTriggerChgRsn_CarModChg);
            }
            break;
        }
    default:
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
            "[RVS]received unexpected VehicleCommSignal, ignoring.");
    }

    return true;
}
void Rvs::EnterRvsStartState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]EnterRvsStartState state:%s-->%s\n", StateToString(old_state).c_str(), StateToString(new_state).c_str());

}

void Rvs::EnterRvsExecuteState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]EnterRvsExecuteState state:%s-->%s, m_triggerType = %d\n", StateToString(old_state).c_str(), StateToString(new_state).c_str(), m_triggerType);

}

void Rvs::EnterRvsConfigState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]EnterRvsConfigState state:%s-->%s, m_triggerType = %d\n", StateToString(old_state).c_str(), StateToString(new_state).c_str(), m_triggerType);

}

void Rvs::EnterRvsIdleState(State old_state, State new_state)
{
    bool ret = false;
    RvsNewSignal_t rvsNewSignal;
    // check signal list, whether had signal need to deal with.
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]EnterRvsIdleState state:%s-->%s, m_triggerType = %d\n", StateToString(old_state).c_str(), StateToString(new_state).c_str(), m_triggerType);
    ret = HandleRvsRequestDequeue(&rvsNewSignal);
    if(true == ret)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s , lijing-test, call HandleRvsNewRequest \n",
                __FUNCTION__);
        HandleRvsNewRequest(rvsNewSignal.signal);
    }
}

void Rvs::EnterRvsUploadState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]EnterRvsUploadState state:%s-->%s, m_triggerType = %d\n", StateToString(old_state).c_str(), StateToString(new_state).c_str(), m_triggerType);

}

void Rvs::EnterRvsStopState(State old_state, State new_state)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]EnterRvsStopState state:%s-->%s, m_triggerType = %d\n", StateToString(old_state).c_str(), StateToString(new_state).c_str(), m_triggerType);

}

bool Rvs::ReceiveTelmDshbData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveTelmDshbData");

    std::shared_ptr<fsm::TelmDshbSiganl> pSignal =
                        std::static_pointer_cast<fsm::TelmDshbSiganl>(signal);
    const OpTelmDshb_NotificationCyclic_Data telmDshbdata = pSignal->GetPayload();

    const OtherTelmDshbSignal_Data &OtherTelmDshb_data = telmDshbdata.otherTelmDshbSignal;
    const TireDeflationDetection_Data &TireDeflation_data = telmDshbdata.tireDeflationDetection;
    const ServiceReminder_Data &ServiceReminder_data = telmDshbdata.serviceReminder;
    const JournalLog_Data &JournalLog_data = telmDshbdata.journalLog;

    //***********Lock Status**************//
    setLockStatus_Data(m_ls, OtherTelmDshb_data);

    //***********Maintenance Status**************//
    setMaintenanceStatus_Data(m_ms, OtherTelmDshb_data);
    setMaintenanceStatus_Data(m_ms, TireDeflation_data);
    setMaintenanceStatus_Data(m_ms, ServiceReminder_data);

    /***********Running Status**************/
	setRunningStatus_Data(m_rs, OtherTelmDshb_data);
	setRunningStatus_Data(m_rs, TireDeflation_data);
	setRunningStatus_Data(m_rs, JournalLog_data);

    /***********Climate Status **************/
    setClimateStatus_Data(m_cs, OtherTelmDshb_data);

    /***********Electric Status **************/
    setElectricStatus_Data(m_es, OtherTelmDshb_data);
    setElectricStatus_Data(m_es, JournalLog_data);

    /***********Pollution Status **************/
    //setPollutionStatus_Data(m_ps, );    //Should be called in CAN message PM25 handler
    
    /***********Additional Status **************/
    setAdditionalStatus_Data(m_AdditionalStatus, OtherTelmDshb_data);
    return true;
}


void Rvs::setBasicVehicleStatus_Data(fsm::BasicVehicleStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
    //data_to. = data_from.
    //Temp for this, the Enum not match. REQ says will change to CAN message.
    data_to.engineStatus = (App_EngineStatus)data_from.engSt1WdSts;
    data_to.speed = data_from.vehSpdIndcd.vehSpdIndcd1;

#if 0
    if(3 == data_from.arg_vehSpdIndcdQly)
    {
        data_to.speedValidity = 1; //true
    }
    else
    {
        data_to.speedValidity = 0; //false
    }
#endif

    //'VIN' get from CAN, here just set value.
    data_to.vin = m_sVin;
    data_to.usageMode = m_carUsgMod;
}


void Rvs::setLockStatus_Data(fsm::LockStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, Lock Status-->Window Status!!!");
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, arg_winPosnStsAtDrv:%d", data_from.winPosnStsAtDrv);
    data_to.winStatusDriver = data_from.winPosnStsAtDrv;
    data_to.winStatusPassenger = data_from.winPosnStsAtPass;
    data_to.winStatusDriverRear = data_from.winPosnStsAtReLe;
    data_to.winStatusPassengerRear = data_from.winPosnStsAtReRi;

    //Hard code as 1, per REQ.
    data_to.winStatusDriverWarning = 1;
    data_to.winStatusPassengerWarning = 1;
    data_to.winStatusDriverRearWarning = 1;
    data_to.winStatusPassengerRearWarning = 1;

    //Windows position
    data_to.winPosDriver = data_from.winPosnStsAtDrv;
    data_to.winPosPassenger = data_from.winPosnStsAtPass;
    data_to.winPosDriverRear = data_from.winPosnStsAtReLe;
    data_to.winPosPassengerRear = data_from.winPosnStsAtReRi;

    //Sunroof open status
    data_to.sunroofOpenStatus = data_from.sunRoofPosnSts;
    //data_to.sunroofPos = ;     //Not defined
    //data_to.sunroofOpenStatusWarning     //Not defined
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, sunroofOpenStatus:%d", data_to.sunroofOpenStatus);

    //Curtain open status
    data_to.curtainOpenStatus = data_from.sunCurtPosnSts;
    data_to.curtainPos  = data_from.sunCurtPosnSts;     //use same as above??
    //data_to.curtainWarning = data_from.sunRoofAndCurtFailr.;     //There are many fields under 'sunRoofAndCurtFailr', use which?

    //tankFlag or chargeLid open status
    data_to.tankFlapStatus = data_from.tankFlapSts;
    data_to.chargeLidRearStatus = data_from.chrgLidRearSts;
    data_to.chargeLidFrontStatus = data_from.chrgLidFrntSts;

    //door open status
    data_to.doorOpenStatusDriver = data_from.doorDrvrSts;
    data_to.doorOpenStatusPassenger = data_from.doorPassSts;
    data_to.doorOpenStatusDriverRear = data_from.doorLeReSts;
    data_to.doorOpenStatusPassengerRear = data_from.doorRiReSts;

    //door lock status
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, arg_doorDrvrLockSts:%d", data_from.doorDrvrLockSts);
    data_to.doorLockStatusDriver = data_from.doorDrvrLockSts;
    data_to.doorLockStatusPassenger = data_from.doorPassLockSts;
    data_to.doorLockStatusDriverRear = data_from.doorLeReLockSts;
    data_to.doorLockStatusPassengerRear = data_from.doorRiReLockSts;

    //door grips status, can't find signal in REQ
    //data_to.doorGripStatusDriver = data_from.doorD
    //data_to.doorGripStatusPassenger   = data_from. ;
    //data_to.doorGripStatusDriverRear   = data_from. ;
    //data_to.doorGripStatusPassengerRear   = data_from. ;

    //trunk or tailgate open status
    ////DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveTelmDshbData, Vehicle Status------>Tailgate Status!!!");
    data_to.trunkLockStatus = data_from.trLockSts;
    data_to.trunkOpenStatus = data_from.trSts;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, trunkLockStatus:%d", data_to.trunkLockStatus);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, trunkOpenStatus:%d", data_to.trunkOpenStatus);

    //engine hood status
    data_to.engineHoodOpenStatus = data_from.hoodSts;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, engineHoodOpenStatus:%d", data_to.engineHoodOpenStatus);

    //Lock Status
    data_to.centralLockingStatus =  data_from.lockgCenSts.lockSt;                    //LY, to check if use "lockSt" field.
    data_to.centralLockingDisStatus = data_from.lockgCenStsForUsrFb;   //LY, change to this per new map
    //data_to.privateLockStatus = data_from.privateLockSts;    //LY, can't find IPCB definitions, seems defined in "OpPrivateLockSts_Notification_Data"
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]LockStatus, centralLockingStatus:%d", data_to.centralLockingStatus);

    //vehicle alarm status
    data_to.vehicleAlarm = data_from.alrmSts.alrmTrgSrc;

    //park brake status
    //data_to.handBrakeStatus   = data_from. ;     //IPCB not defined
    data_to.electricParkBrakeStatus  = data_from.epbSts.epbSts1;
}

void Rvs::setMaintenanceStatus_Data(fsm::MaintenanceStatus_Data &data_to, const TireDeflationDetection_Data &data_from)
{
    data_to.tyreStatusDriver = data_from.leFrntTireMsg.pWarnFlg;     //LY, temp use 'pWarnFlg' field.
    data_to.tyreStatusPassenger = data_from.riFrntTireMsg.pWarnFlg;
    data_to.tyreStatusDriverRear = data_from.leReTireMsg.pWarnFlg;
    data_to.tyreStatusPassengerRear = data_from.riReTireMsg.pWarnFlg;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]setMaintenanceStatus, tyreStatusDriver:%d", data_to.tyreStatusDriver);
}

void Rvs::setMaintenanceStatus_Data(fsm::MaintenanceStatus_Data &data_to, const ServiceReminder_Data &data_from)
{
    data_to.engineHrsToService = data_from.engHrToSrv;
    //data_to. = data_from.;
    data_to.distanceToService = data_from.dstToSrv;
    data_to.daysToService = data_from.dayToSrv;

    if((1 == data_from.srvTrig) && (0 == old_srvTrig))
    {
        m_needUploadTimes++;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]MaintenanceStatus, srvTrig change to 1");
    }
    old_srvTrig = data_from.srvTrig;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]MaintenanceStatus, engineHrsToService:%d", data_to.engineHrsToService);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]MaintenanceStatus, distanceToService:%d", data_to.distanceToService);
}

void Rvs::setMaintenanceStatus_Data(fsm::MaintenanceStatus_Data &data_to, const OtherTelmDshbSignal_Data  &data_from)
{
    //engine oil pressure
    data_to.engineOilLevelStatus = data_from.engOilLvlSts;
    //m_as.runningStatus.engineOilTemperature = ??;    //LY, not defined yet.
    data_to.engineOilPressureWarning = data_from.engOilPWarn;
    data_to.engineCoolantTemperature = data_from.engT.engT1;      //Temp use this field. Need REQ confirm with customer

    //engine coolant level status
    data_to.engineCoolantLevelStatus = data_from.engCooltLvl;
    data_to.brakeFluidLevelStatus  = data_from.brkFldLvl;
    data_to.washerFluidLevelStatus  = data_from.wshrFldTankStsToHMI;
    data_to.indcrTurnLeWarning  = data_from.fltIndcrTurnLeRe;
    data_to.indcrTurnRiWarning  = data_from.fltIndcrTurnRiRe;
    data_to.indcrTurnOnPassSideWarning  = data_from.fltOfIndcrTurnOnPassSide;
    data_to.indcrTurnOnDrvrSideWarning  = data_from.fltOfIndcrTurnOnDrvrSide;
    data_to.indcrTurnLeFrntWarning  = data_from.fltIndcrTurnLeFrnt;
    data_to.fogLiReWarning  = data_from.fltOfFogLiRe;
    //data_to.bulbStatus  = ??.    //Not defined

}

void Rvs::setClimateStatus_Data(fsm::ClimateStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
	data_to.interiorTemp = data_from.cmptmtTFrnt.cmptmtTFrnt1;

	//To do, need check

	data_to.exteriorTemp = getMetricTemp(data_from.ambTIndcdWithUnit.ambTIndcd, data_from.ambTIndcdWithUnit.ambTIndcdUnit);

	//data_to.preClimateActive = data_from. ;

	//data_to.airCleanSts = data_from. ;   //REQ NOT send this.
/*
	data_to.drvHeatSts = data_from. ;
	data_to.passHeatingSts = data_from. ;
	data_to.rlHeatingSts = data_from. ;
	data_to.rrHeatingSts = data_from. ;
	data_to.drvVentSts = data_from. ;
	data_to.passVentSts = data_from. ;
	data_to.rrVentSts = data_from. ;
	data_to.rlVentSts = data_from. ;
*/

	//data_to.interCO2Warning = data_from. ;     //OpInCarCo2HighWarn_Notification_Data 

/*  CAN not ready
	data_to.fragStrs. = data_from. ;
	data_to.fragStrs. = data_from. ;
	data_to.fragStrs. = data_from. ;
	data_to.fragStrs. = data_from. ;
	data_to.fragStrs. = data_from. ;
	data_to.fragStrs. = data_from. ;
*/

}


void Rvs::setElectricStatus_Data(fsm::ElectricStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
	//data_to.isCharging = data_from. ;    	//Need "OnBdChrgrSts_Notification_Data"
	//data_to.isPluggedIn = data_from. ;	//Need "OnBdChrgrSts_Notification_Data"

	//data_to.stateOfCharge = data_from. ;   //Not send this.

	data_to.chargeLevel = data_from.dispHvBattLvlOfChrg;

	//data_to.timeToFullyCharged = data_from. ;		//Need "BookChrgnDisplayOption_NotificationCyclic_Data"

	//data_to.statusOfChargerConnection = data_from. ;  //Not send this.
	//data_to.chargerState = data_from. ;				//Not send this.

	//Note: where to set data_from.dstEstimdToEmptyForDrvgElecIndcd.dstUnit?
	data_to.distanceToEmptyOnBatteryOnly = data_from.dstEstimdToEmptyForDrvgElecIndcd.dstToEmpty;

  	//REQ NOT require those.
/*
	data_to.ems48VSOC = data_from. ;
	data_to.ems48VDisSOC = data_from. ;
	data_to.emsHVRBSError = data_from. ;
	data_to.emsHVDC = data_from. ;
	data_to.emsRBSModeDisplay = data_from. ;
	data_to.emsOVPActive = data_from. ;
	data_to.ems48vPackTemp1 = data_from. ;
	data_to.ems48vPackTemp2 = data_from. ;
	data_to.emsBMSLBuildSwVersion = data_from. ;
	data_to.emsCBRemainingLife = data_from. ;
	data_to.chargeHvSts = data_from. ;
*/
	data_to.ptReady = data_from.onBdChrgrSt;
	data_to.dcDcActvd = data_from.dcDcActvd;
	//data_to.dcDcConnectStatus = data_from.dc ;     //Can't find IP command
	data_to.wptObjt = data_from.wPTObj;
	data_to.wptFineAlignt = data_from.wPTFineAlignt;

	//data_to.wptActived = data_from.book ;   //BookChrgnActvd_SetRequestNoReturn_Data

	data_to.chargeIAct = data_from.onBdChrgrIAct;
	data_to.chargeUAct = data_from.onBdChrgrUAct;
	data_to.chargeSts = data_from.wPTChrgrSts;
	data_to.disChargeConnectStatus = data_from.onBdChrgrHndlSts1;
	data_to.disChargeIAct = data_from.dChaIAct;
	data_to.disChargeUAct = data_from.dChaUAct;
	data_to.disChargeSts = data_from.onBdChrgrSt;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ElectricStatus, chargeLevel:%d", data_to.chargeLevel);
}

void Rvs::setElectricStatus_Data(fsm::ElectricStatus_Data &data_to, const JournalLog_Data &data_from)
{
	data_to.averPowerConsumption = data_from.pwrCnsAvgIndcd.pwrCns;
	data_to.indPowerConsumption = data_from.pwrCnsIndcd.pwrCns;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ElectricStatus, averPowerConsumption:%d", data_to.averPowerConsumption);
}

void Rvs::setRunningStatus_Data(fsm::RunningStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
    uint32_t unit;
    data_to.odometer = data_from.dstTrvldMst2;      //In Miles
    data_to.fuelLevel = data_from.fuLvlIndcd.fuLvlValFromFuTbl;
    data_to.fuelLevelStatus = data_from.fuLvlLoIndcn;
    data_to.fuelEnLevel = data_from.engOilLvl;
    data_to.fuelEnCnsFild = data_from.engFuCnsFild;
    data_to.fuelEnCns = data_from.engFuCns;
    data_to.fuelLow1WarningDriver = data_from.fuLvlLoWarn;
    data_to.fuelLow2WarningDriver = data_from.fuLvlLoIndcn;

    unit = data_from.dstToEmptyIndcd.dstUnit;
    data_to.distanceToEmpty = getMetricDistance(data_from.dstToEmptyIndcd.dstToEmpty, unit);

    data_to.trvlDistance = data_from.dstTrvldHiResl;

    //data_to.avgSpeedLatestDrivingCycle = data_from.;     //Not defined
    //

    data_to.notifForEmgyCallStatus = data_from.notifForEmgyCall;

    data_to.seatBeltStatusDriver = data_from.bltLockStAtDrvr.bltLockSt1;
    data_to.seatBeltStatusPassenger = data_from.bltLockStAtPass.bltLockSt1;
    data_to.seatBeltStatusDriverRear = data_from.bltLockStAtRowSecLe.bltLockSt1;
    data_to.seatBeltStatusPassengerRear = data_from.bltLockStAtRowSecRi.bltLockSt1;
    data_to.seatBeltStatusMidRear = data_from.bltLockStAtRowSecMid.bltLockSt1;
    data_to.seatBeltStatusThDriverRear = data_from.bltLockStAtRowThrdLe.bltLockSt1;
    data_to.seatBeltStatusThPassengerRear = data_from.bltLockStAtRowThrdRi.bltLockSt1;
    //data_to.seatBeltStatusThDriverMid = data_from.;    //Not defined
    data_to.brakePedalDepressed = data_from.brkFldLvl;
    data_to.gearManualStatus = data_from.gearIndcnRec.gearIndcn;   //To check if use 'gearIndcn'?
    data_to.gearAutoStatus = data_from.gearLvrIndcn;
    //brakePedalDepressedValidity no VDS defintion

    data_to.engineSpeed = data_from.engSpdDispd;
    //data_to.transimissionGearPostion = data_from.;  //Not defined
    //data_to.cruiseControlStatus = data_from.;   //Not defined
    //data_to.engineBlockedStatus = data_from.;  //Not defined

    data_to.tripMeter1 = data_from.dstTrvld1;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]RunningStatus, notifForEmgyCallStatus:%d", data_to.notifForEmgyCallStatus);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]RunningStatus, engineSpeed:%d", data_to.engineSpeed);
}

void Rvs::setRunningStatus_Data(fsm::RunningStatus_Data &data_to, const JournalLog_Data &data_from)
{
	data_to.aveFuelConsumption = data_from.fuCnsAvgIndcd.fuCnsIndcdVal;

	data_to.aveFuelConsumptionUnit_presence = false;     //Not send this. REQ NOT clear.
	//data_to.aveFuelConsumptionUnit = data_from.fuCnsAvgIndcd.fuCnsIndcdUnit;
	
	//data_to.aveFuelConsumptionInLatestDrivingCycle = data_from.;    //Not send this in E3.
	data_to.avgSpeed = data_from.vehSpdAvgIndcd.vehSpbIndcd;
	
	data_to.indFuelConsumption = data_from.fuCnsIndcd.fuCnsIndcdVal;   //Note, need change to Eng.
	data_to.tripMeter2 = data_from.dstTrvld2;   //REQ say in "OtherTelmDshbSignal_Data"
	DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]RunningStatus, avgSpeed:%d", data_to.avgSpeed);
}

void Rvs::setRunningStatus_Data(fsm::RunningStatus_Data &data_to, const TireDeflationDetection_Data &data_from)
{
	data_to.tyrePreWarningDriver = data_from.leFrntTireMsg.pWarnFlg;
	data_to.tyrePreWarningPassenger = data_from.riFrntTireMsg.pWarnFlg;
	data_to.tyrePreWarningDriverRear = data_from.leReTireMsg.pWarnFlg;
	data_to.tyrePreWarningPassengerRear = data_from.riReTireMsg.pWarnFlg;
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]RunningStatus, tyrePreWarningDriver:%d", data_to.tyrePreWarningDriver);
}


void Rvs::setPollutionStatus_Data(fsm::PollutionStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
    //TO do, need add CAN message.
#if 0
    data_to.interiorPM25 =
    data_to.exteriorPM25 =
    data_to.interiorPM25Level =
    data_to.exteriorPM25Level =
#endif
}



//This not implemented.
void Rvs::setAdditionalStatus_Data(fsm::AdditionalStatus_Data &data_to, const OtherTelmDshbSignal_Data &data_from)
{
#if 0
    data_to.confAndId =
    data_to.
    data_to.
    data_to.
#endif
}


//LY, will delete this
bool Rvs::ReceiveVehSoHCheckData(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveVehSoHCheckData");

    std::shared_ptr<fsm::VehSoHCheckSignal> vehSoHCheckSignal =
        std::static_pointer_cast<fsm::VehSoHCheckSignal>(signal);
    const VehSoHCheckData data = vehSoHCheckSignal->GetPayload();

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveVehSoHCheckData, Response srvTrig:%d", data.srvTrig);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveVehSoHCheckData, dayToSrv:%d", data.dayToSrv);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveVehSoHCheckData, engHrToSrv:%d", data.engHrToSrv);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveVehSoHCheckData, dstToSrv:%d", data.dstToSrv);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]ReceiveVehSoHCheckData, srvRst:%d", data.srvRst);
    m_as.maintenanceStatus.distanceToService = data.dstToSrv;
    return true;
}

bool Rvs::SendVehSoHCheckRequest()
{
    bool ret = false;
    uint64_t  request_id = 0;
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]SendVehSoHCheckRequest");
    fsm::IpCommandBrokerSignalSource& ipcb_signal_source = fsm::IpCommandBrokerSignalSource::GetInstance();
    ret = ipcb_signal_source.VehicleSoHCheckRequest(request_id);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, VehicleSoHCheckRequest ret = %d", __FUNCTION__, ret);
    return ret;
}

bool Rvs::RequestPosition(std::shared_ptr<fsm::TransactionId> tranId)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Rvs::RequestPosition");
    std::shared_ptr<fsm::VpomTransactionId> vpom_request_id = std::dynamic_pointer_cast<fsm::VpomTransactionId>(tranId);

    if(vpom_request_id == nullptr)
    {
        vpom_request_id = std::make_shared<fsm::VpomTransactionId>();
    }

    bool ret_val = true;
    VpomIPositioningService& vpom_gnss_service_object = fsm::VpomSignalSource::GetInstance().GetPositioningServiceObject();
    MapTransactionId(vpom_request_id);
    ret_val = ret_val && vpom_gnss_service_object.GNSSPositionDataRawRequest(vpom_request_id->GetId());
    if(!ret_val)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "[RVS]RequestPosition, request position information fail !!!!\n");
    }

    // start Timer, for request position timeout
    RemoveTimeout(m_kRvs_request_position_tick_id);
    m_kRvs_request_position_tick_id = RequestTimeout(static_cast<std::chrono::seconds>(VPOM_POSITION_RESPONSE_TIMEOUT), false);
    MapTransactionId(m_kRvs_request_position_tick_id.GetSharedCopy());

    return ret_val;
}


/************************************************************/
// @brief :request car mode
// @param[in]  
// @return     True if request ok
// @author     Nie Yujin
/************************************************************/
bool Rvs::RequestVinNumber()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    
    bool ret = true;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
        std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

    //request car mode
    if (igen.Request_VINNumber(vehicle_comm_request_id->GetId()) == vc::RET_OK)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
            "Requests for vinNumber sent to VehicleComm.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
            "Failed to request vinNumber from VehicleComm");
        ret = false;
    }
    return ret;
}

/************************************************************/
// @brief :handle kVINNumberSignal
// @param[in]  received signals
// @return     True if transaction is not yet finished, false if it is finished.
// @author     Nie Yujin
/************************************************************/
bool Rvs::HandleVinNumberSignal(std::shared_ptr<fsm::Signal> signal)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);
    
    bool ret = true;
    std::shared_ptr<fsm::VINNumberSignal> vinNumberSignal = std::static_pointer_cast<fsm::VINNumberSignal>(signal);

    if(vinNumberSignal)
    {
        if(vinNumberSignal->GetData()->vin_number[0] == 0xFF) // no vin_number exists
        {
           DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Vin number invalid.");
        }
        else
        {
            m_sVin = vinNumberSignal->GetData()->vin_number;
        }
    }
    else
    {
       DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "Get vin number fail.");
    }
    return ret;
}

/**
* \brief Request Car mode and Usage mode from vehicle_comm
*
* \param none
*
* \return True if request success, false if error occured
*/
bool Rvs::RequestCarUsageMode()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    bool ret = true;
    std::shared_ptr<fsm::VehicleCommTransactionId> vehicle_comm_request_id =
        std::make_shared<fsm::VehicleCommTransactionId>();
    MapTransactionId(vehicle_comm_request_id);

    fsm::VehicleCommSignalSource& vc_signal_source = fsm::VehicleCommSignalSource::GetInstance();
    vc::IGeneral& igen = vc_signal_source.GetVehicleCommClientObject().GetGeneralInterface();

    //request car mode
    if (igen.Request_CarMode(vehicle_comm_request_id->GetId()) == vc::RET_OK)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
            "[RVS], requests for car mode sent to VehicleComm.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
            "[RVS], failed to request car mode from VehicleComm");
        ret = false;
    }

    //request usage mode
    if (igen.Request_CarUsageMode(vehicle_comm_request_id->GetId()) == vc::RET_OK)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
            "[RVS], requests for usage mode sent to VehicleComm.");
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
            "[RVS], failed to request usage mode from VehicleComm");
        ret = false;
    }

    return ret;
}

/**
* \brief Request Config Value:RVS_INTERVAL
*
* \param none
*
* \return True if request success, false if error occured
*/
bool Rvs::UpdateConfiguration()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s", __FUNCTION__);

    bool ret = true;
    unsigned int value = 0;
    lcfg::ILocalConfigPersistMgr* iLocalConfig = lcfg::ILocalConfigPersistMgr::GetInstance();
    ret = iLocalConfig->GetCfgValue("status-trigger_RVS_INTERVAL", value);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS], requests for RVS_INTERVAL, ret = %d, value = %d", ret, value);

    if (ret && (value>0))
    {
        m_uploadInterval = value;
    }
    else
    {
        m_uploadInterval = RVS_INTERVAL;
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "[RVS], failed to request Configuration");
    }
    return ret;
}

void Rvs::UpdateConfigType(char* configString)
{
    if(configString == NULL)
    {
        return;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, the whole string configString= %s", configString);

    //rest m_configType
    m_configType = kRvs_config_empty;

    //std::shared_ptr<char> parsedString;
    char* parsedString;
    parsedString = strstr(configString, "bs");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_bs;
    }

    parsedString = strstr(configString, ",bs");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_bs;
    }

    parsedString = strstr(configString, "ls");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ls;
    }

    parsedString = strstr(configString, ",ls");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ls;
    }

        parsedString = strstr(configString, "ms");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ms;
    }

    parsedString = strstr(configString, ",ms");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ms;
    }

    parsedString = strstr(configString, "rs");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_rs;
    }

    parsedString = strstr(configString, ",rs");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_rs;
    }

    parsedString = strstr(configString, "cs");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_cs;
    }

    parsedString = strstr(configString, ",cs");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_cs;
    }

    parsedString = strstr(configString, "es");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_es;
    }

    parsedString = strstr(configString, ",es");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_es;
    }

    parsedString = strstr(configString, "ps");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ps;
    }

    parsedString = strstr(configString, ",ps");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ps;
    }


    parsedString = strstr(configString, "ts");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ts;
    }

    parsedString = strstr(configString, ",ts");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_ts;
    }

    parsedString = strstr(configString, "cc");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_cc;
    }

    parsedString = strstr(configString, ",cc");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType |= kRvs_cc;
    }


    parsedString = strstr(configString, "as");
    if(parsedString != NULL)
    {
        parsedString = strstr(parsedString, "as.");
        if(parsedString == NULL)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
            m_configType |= kRvs_as;
        }
    }

    parsedString = strstr(configString, ",as");
    if(parsedString != NULL)
    {
        parsedString = strstr(parsedString, ",as.");
        if(parsedString == NULL)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
            m_configType |= kRvs_as;
        }
    }

    parsedString = strstr(configString, "as.ci");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType = m_configType|kRvs_as_ci;
    }

    parsedString = strstr(configString, ",as.ci");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType = m_configType|kRvs_as_ci;
    }


    parsedString = strstr(configString, "as.ew");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType = m_configType|kRvs_as_ew;
    }

    parsedString = strstr(configString, ",as.ew");
    if(parsedString != NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, parsedString = %s\n", parsedString);
        m_configType = m_configType|kRvs_as_ew;
    }


    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]UpdateConfigType, m_configType= %d, 0x%x", m_configType, m_configType);
}

bool Rvs::CheckWhetherExistConfigType(int CurrentConfigTypeSum, int targetConfigType)
{
    int checkResult = 0;

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]CheckWhetherExistConfigType, CurrentConfigTypeSum = 0x%x, targetConfigType = 0x%x",CurrentConfigTypeSum, targetConfigType);
    checkResult = CurrentConfigTypeSum&targetConfigType;
    if(checkResult== 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
* \brief Update trigger type if has new reason
*
* \param[in] reason
*
* \return none
*/
void Rvs::UpdateTriggerType(RVSTriggerTypesChangeReason reason)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS]%s, m_triggerType = %d, reason = %d", __FUNCTION__, m_triggerType, reason);
    RVSTriggerTypes triggerType = m_triggerType;

    switch (reason)
    {
    case kTriggerChgRsn_CarModChg:
        switch (m_triggerType)
        {
        case kTriggerType_StartDriving:
        case kTriggerType_InDriving:
            // out of driving mode
            if ((vc::CAR_NORMAL != m_carMod) || (vc::CAR_DRIVING != m_carUsgMod))
            {
                triggerType = kTriggerType_StopDriving;
            }
            break;
        case kTriggerType_Unknown:
        case kTriggerType_StopDriving:
        default:
            // into dirving mode
            if ((vc::CAR_NORMAL == m_carMod) && (vc::CAR_DRIVING == m_carUsgMod))
            {
                triggerType = kTriggerType_StartDriving;
            }
            break;
        }
        break;
    case kTriggerChgRsn_Uploaded:
        if (kTriggerType_StartDriving == m_triggerType)
        {
            triggerType = kTriggerType_InDriving;
        }
        else if (kTriggerType_StopDriving == m_triggerType)
        {
            SetState(kRvsIdle);
        }
        else
        {
            // nothing to do
        }
        break;
    case kTriggerChgRsn_TimerChg:
        // if timer already started, need to restart
        if ((kTriggerType_StartDriving == m_triggerType) ||(kTriggerType_InDriving == m_triggerType))
        {
            RemoveTimeout(m_kRvs_period_tick_id);
            m_kRvs_period_tick_id = RequestTimeout(static_cast<std::chrono::seconds>(m_uploadInterval*60), true);
            MapTransactionId(m_kRvs_period_tick_id.GetSharedCopy());
        }
        break;
    case kTriggerChgRsn_Timeout:
        break;
    default:
        break;
    }

    if (m_triggerType != triggerType)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "[RVS], triggerType changed from %d to %d",m_triggerType, triggerType);

        // update trigger tiye
        m_triggerType = triggerType;

        switch (m_triggerType)
        {
        case kTriggerType_StartDriving:
        {
            // go to driving, need to collect and upload data
            SetState(kRvsExecute);
            std::shared_ptr<fsm::Signal> RvsSignalCollect = std::make_shared<RvsSignalCollectVehicleState>();
            EnqueueSignal(RvsSignalCollect);

            m_needUploadTimes++;

            // start timer for uploading data interval
            RemoveTimeout(m_kRvs_period_tick_id);
            m_kRvs_period_tick_id = RequestTimeout(static_cast<std::chrono::seconds>(m_uploadInterval*60), true);
            MapTransactionId(m_kRvs_period_tick_id.GetSharedCopy());
            break;
        }
        case kTriggerType_InDriving:
            // nothing to do
            break;
        case kTriggerType_StopDriving:
        {
            // End of driving, need to collect and upload data
            SetState(kRvsExecute);
            std::shared_ptr<fsm::Signal> RvsSignalCollect = std::make_shared<RvsSignalCollectVehicleState>();
            EnqueueSignal(RvsSignalCollect);

            m_needUploadTimes++;

            // stop the interval timer
            RemoveTimeout(m_kRvs_period_tick_id);
            break;
        }
        case kTriggerType_Unknown:
        default:
            // nothing to do
            break;
        }
    }
}

int32_t Rvs::getMetricTemp(int32_t value, uint32_t unit)
{
	return value;
}

int32_t Rvs::getMetricDistance(uint16_t input_data, uint32_t unit)
{
    int32_t result;
	if (0x1 == unit) 	// miles
	{
		result = input_data * 1.609; // change from miles to km;
	}
	else
	{
		result = input_data;
	}
	return result;
}

int32_t Rvs::getMetricSpeed(uint16_t input_data, uint32_t unit)
{
    int32_t result;
    if (1 == unit) // 1:mph
    {
        // change to kmph
        result = input_data * 1.609;
    }
    else //0:kmph, others
    {
        result = input_data;
    }
    return result;
}

void Rvs::ComposeRVSSimuData(Rvs_TS_t  &ts)
{
    memcpy(ts.vin, "12345678901234567", 17);
    ts.powerSource = 1;
    ts.backupBattery.stateOfCharge = 1;
    ts.backupBattery.chargeLevel = 0;
    ts.backupBattery.stateOfHealth = 1;
    ts.backupBattery.voltage = 0;
    ts.powerMode = 2;
    ts.sleepCycleNextWakeupTime.seconds = 12;
    ts.sleepCycleNextWakeupTime.milliseconds = 13;
    ts.serviceProvisoned = 1;
    ts.healthStatus = 0;
    //ts.diagnostics.ecuID[32]
    //ts.diagnostics.diagnosticResult
    //ts.diagnostics.list
    //ts.diagnostics.exceptionCode
    //ts.networkAccessStatus;
    memcpy(ts.serialNumber, "1111111111", 10);
    memcpy(ts.completeECUPartNumbers, "1111111111", 10);
    memcpy(ts.imei, "12345678901234567", 17);
    memcpy(ts.hwVersion, "1.010.1", 7);
    memcpy(ts.swVersion, "12345678901234567", 17);
    memcpy(ts.mpuVersion, "12345678901234568", 17);
    memcpy(ts.mcuVersion, "12345678901234569", 17);
}

void Rvs::ComposeRVSSimuData(Rvs_AS_DS_t  &as_ds)
{
    as_ds.srsStatus = 1;//error
    as_ds.vehicleAlarm = 2;//error
    as_ds.doorOpenStatusDriver = 3;//error
    as_ds.doorOpenStatusPassenger = 4;
    as_ds.doorOpenStatusDriverRear = 5;
    as_ds.doorOpenStatusPassengerRear = 6;
    as_ds.doorLockStatusDriver = 7;
    as_ds.doorLockStatusPassenger =8;
    as_ds.doorLockStatusDriverRear = 9;
    as_ds.doorLockStatusPassengerRear =0;
    as_ds.trunkOpenStatus = 7;
    as_ds.trunkLockStatus = 1;
    as_ds.engineHoodOpenStatus = 3;
    as_ds.centralLockingStatus = 4;
    as_ds.seatBeltStatusDriver = 1;
    as_ds.seatBeltStatusPassenger = 1;
    as_ds.seatBeltStatusDriverRear  = 1;
    as_ds.seatBeltStatusPassengerRear = 1;
    as_ds.handBrakeStatus = 9;//error
    as_ds.electricParkBrakeStatus = 0;//error
    as_ds.electricParkBrakeStatusValidity = 1;//error
}

void Rvs::ComposeRVSSimuData(Rvs_AS_MS_t  &as_ms)
{
    as_ms.mainBatteryStatus.stateOfCharge = 1;
    as_ms.mainBatteryStatus.chargeLevel = 2;
    as_ms.mainBatteryStatus.stateOfHealth = 3;
    as_ms.mainBatteryStatus.voltage = 4;
    as_ms.tyreStatusDriver = 104;
    as_ms.tyreStatusPassenger = 105;
    as_ms.tyreStatusDriverRear = 106;
    as_ms.tyreStatusPassengerRear = 107;
    as_ms.tyrePreWarningDriver = 108;
    as_ms.tyrePreWarningPassenger = 109;
    as_ms.tyrePreWarningDriverRear = 110;
    as_ms.tyrePreWarningPassengerRear = 111;
    as_ms.engineHrsToService = 112;
    as_ms.distanceToService = 113;
    as_ms.daysToService = 114;
    as_ms.serviceWarningStatus = 115;
    as_ms.serviceWarningTrigger = 2;
    as_ms.odometer = 117;
    as_ms.brakeFluidLevelStatus = 118;
    as_ms.washerFluidLevelStatus = 119;
}

void Rvs::ComposeRVSSimuData(Rvs_AS_RS_t  &as_rs)
{
    as_rs.engineOilLevelStatus = 201;
    as_rs.engineOilTemperature = 202;
    as_rs.engineOilPressureWarning = 203;
    as_rs.engineCoolantTemperature = 204;
    as_rs.engineCoolantTemperatureValidity = 1;
    as_rs.engineCoolantLevelStatus = 206;
    as_rs.fuelLevel = 207;
    as_rs.fuelLevelStatus = 208;
    as_rs.aveFuelConsumption = 209;
    as_rs.aveFuelConsumptionInLatestDrivingCycle = 210;
    as_rs.avgSpeed = 211;
    as_rs.tripMeter1 = 212;
    as_rs.tripMeter2 = 213;
    as_rs.bulbStatus = 214;
}

void Rvs::ComposeRVSSimuData(Rvs_AS_ES_t  &as_es)
{
    as_es.isCharging = 1;
    as_es.isPluggedIn = 1;
    as_es.stateOfCharge = 501;
    as_es.chargeLevel = 502;
    as_es.timeToFullyCharged = 503;
    as_es.statusOfChargerConnection = 1;
    as_es.chargerState = 1;
    as_es.distanceToEmptyOnBatteryOnly = 506;
    as_es.ems48VSOC = 507;
    as_es.emsHVRBSError = 1;
    as_es.emsRBSModeDisplay = 3;
    as_es.emsOVPActive = 1;
    as_es.ems48vPackTemp1 = 1;
    as_es.ems48vPackTemp2 = 1;
    //uint8_t emsBMSLBuildSwVersion[32];
    as_es.emsCBRemainingLife = 1;
    as_es.chargeHvSts = 3;
    as_es.ptReady = 2;
    as_es.averPowerConsumption = 1;
    as_es.chargeSts = 2;
}

void Rvs::ComposeRVSSimuData(Rvs_AS_CS_t  &as_cs)
{
    as_cs.winStatusDriver = 1;
    as_cs.winStatusDriverValidity = 1;
    as_cs.winStatusPassenger = 3;
    as_cs.winStatusPassengerValidity = 1;
    as_cs.winStatusDriverRear = 5;
    as_cs.winStatusDriverRearValidity = 1;
    as_cs.winStatusPassengerRear = 7;
    as_cs.winStatusPassengerRearValidity = 1;
    as_cs.sunroofOpenStatus = 9;
    as_cs.sunroofOpenStatusValidity = 0;
    as_cs.ventilateStatus = 1;
    as_cs.winPosDriver = 2;
    as_cs.winPosPassenger = 3;
    as_cs.winPosDriverRear = 4;
    as_cs.winPosPassengerRear = 5;
    as_cs.interiorTemp = 6;
    as_cs.exteriorTemp = 7;
    as_cs.exteriorTempValidity = 1;
    as_cs.preClimateActive = 1;
    as_cs.airCleanSts = 0;
    as_cs.winCloseReminder = 1;
    as_cs.drvHeatSts = 2;
    as_cs.passHeatingSts = 3;
    as_cs.rlHeatingSts = 4;
    as_cs.rrHeatingSts = 5;
    as_cs.drvVentSts = 6;
    as_cs.passVentSts = 7;
    as_cs.rrVentSts = 1;
    as_cs.rlVentSts = 2;
}

void Rvs::ComposeRVSSimuData(Rvs_AS_DB_t  &as_db)
{
    as_db.brakePedalDepressed = 1;
    as_db.brakePedalDepressedValidity = 1;
    as_db.engineSpeed = 403;
    as_db.engineSpeedValidity = 1;
    as_db.transimissionGearPostion = 405;
    as_db.cruiseControlStatus = 406;
}

}
