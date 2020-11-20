///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file gly_vds_rce_signal.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface

// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   22-Jan-2019
///////////////////////////////////////////////////////////////////

#include "signals/geely/gly_vds_rce_signal.h"
#include "signals/geely/gly_vds_res_signal.h"
#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/rce_signal_adapt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{
    GlyVdsRceSignal::GlyVdsRceSignal(fsm::VdServiceTransactionId& transaction_id, void* vdsService):
            fsm::VdmSignal(transaction_id, fsm::kVDServiceRce)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, Construction function", __FUNCTION__);
        bool result = UnpackPayload(vdsService);
        if(result){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, payload success", __FUNCTION__);
        } else {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "GlyVdsRceSignal::%s, payload failed", __FUNCTION__);
        }
    }

    std::shared_ptr<fsm::Signal> GlyVdsRceSignal::CreateGlyVdsRceSignal (fsm::VdServiceTransactionId& transaction_id, void* vdsService)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s", __FUNCTION__ );
        GlyVdsRceSignal* signal = new GlyVdsRceSignal(transaction_id, vdsService);
        return std::shared_ptr<GlyVdsRceSignal>(signal);
    }

    GlyVdsRceSignal::~GlyVdsRceSignal()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, destruct", __FUNCTION__ );
    }

    /*bool GlyVdsRceSignal::SetHeadPayload()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s.", __FUNCTION__ );
        if (m_pVDSRes)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s free_struct.m_pVDSRes addr:%0x\n", __FUNCTION__, m_pVDSRes);
            delete m_pVDSRes;
        }
        m_pVDSRes = new VDServiceRequest();
        VdmSignal::SetHeadPayload(m_pVDSRes);
        if(m_pVDSReq == nullptr ||
        m_pVDSReq->header == nullptr ||
        m_pVDSReq->header->eventId == nullptr)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s m_pVdServiceRequest->header->eventId == nullptr.\n", __FUNCTION__);
            return false;
        }
        else
        {
            m_pVDSRes->header->optional.setPresence(asn_eventId);
            m_pVDSRes->header->eventId = new TimeStamp();
            m_pVDSRes->header->eventId->seconds = m_pVDSReq->header->eventId->seconds;
            m_pVDSRes->header->eventId->optional.setPresence(asn_milliseconds);
            m_pVDSRes->header->eventId->milliseconds= m_pVDSReq->header->eventId->milliseconds;
        }
        return true;
    }*/

    int GlyVdsRceSignal::PackGeelyAsn(void *vdServiceRequest)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s.", __FUNCTION__ );
        if (nullptr != m_pRMT_ClimateStrtStopRes){
            RceSignalAdapt _rce_adaptor(vdServiceRequest);
            bool ret = _rce_adaptor.SetStrtStopResultPayload(*m_pRMT_ClimateStrtStopRes);
            return (ret == true? 0 : 1);
        } else if (nullptr != m_pRMT_ClimateAutoStopRes){
            RceSignalAdapt _rce_adaptor(vdServiceRequest);
            bool ret = _rce_adaptor.SetAutoStopResultPayload(*m_pRMT_ClimateAutoStopRes);
            return (ret == true? 0 : 1);
        } else if (nullptr != m_pRMT_RemoteStrtErrRes){
            RceSignalAdapt _rce_adaptor(vdServiceRequest);
            bool ret = _rce_adaptor.SetErrorResultPayload(*m_pRMT_RemoteStrtErrRes);
            return (ret == true? 0 : 1);
        }
        return -1;
    }

    std::shared_ptr<RMT_ClimatizationRequest_t> GlyVdsRceSignal::GetRequestPayload()
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s.", __FUNCTION__ );
        if (nullptr == m_climateReq.get()){
            return nullptr;
        }
        return m_climateReq;
    }

    bool GlyVdsRceSignal::SetResultPayload(RMT_ClimatizationResponse_t res)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s.", __FUNCTION__ );
        m_pRMT_ClimateAutoStopRes = nullptr;
        m_pRMT_ClimateStrtStopRes = nullptr;
        m_pRMT_RemoteStrtErrRes = nullptr;
        RequestHeader_Data _heade;
        if(nullptr == m_pRMT_ClimatizationReq.get()) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineReq.get().\n", __FUNCTION__);
            // TODO creat a header
            return false;
        } else {
            _heade = m_pRMT_ClimatizationReq->header;
        }


        switch (res.m_resType){
        case ResType_StrtStop:
        {
            m_pRMT_ClimateStrtStopRes.reset(new fsm::RMT_ClimateSrtStopResult_t());
            if (nullptr == m_pRMT_ClimateStrtStopRes.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_ClimateStrtStopRes.\n", __FUNCTION__);
                return false;
            }
            if (vc::ErsStrtApplSts_ErsStsOff == res.m_engine_state){ // stop status
                m_pRMT_ClimateStrtStopRes->engine_status = app_EngineStatus_engine_off;
            } else if (vc::ErsStrtApplSts_ErsStsRunng == res.m_engine_state){ //runng status
                m_pRMT_ClimateStrtStopRes->engine_status = app_EngineStatus_engine_running;
            } else { //other status Noset/strtg
                m_pRMT_ClimateStrtStopRes->engine_status = app_EngineStatus_unknown;
            }
            m_pRMT_ClimateStrtStopRes->operationSucceeded = res.m_operateSuccess;
            m_pRMT_ClimateStrtStopRes->speed = res.m_speed;
            m_pRMT_ClimateStrtStopRes->chargeHvSts = res.m_chargeHvSts;
            m_pRMT_ClimateStrtStopRes->header = _heade;
            break;
        }
        case ResType_AutoStop:
        {
            m_pRMT_ClimateAutoStopRes.reset(new fsm::RMT_CliamteAutoStopResult_t());
            if (nullptr == m_pRMT_ClimateAutoStopRes.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_ClimateAutoStopRes.\n", __FUNCTION__);
                return false;
            }
            m_pRMT_ClimateAutoStopRes->operationSucceeded = res.m_operateSuccess;
            m_pRMT_ClimateAutoStopRes->preClimateActive = res.m_active;
            m_pRMT_ClimateAutoStopRes->header = _heade;
            break;
        }
        case ResType_Err:
        {
            m_pRMT_RemoteStrtErrRes.reset(new fsm::RMT_RemoteStartCommErrorResult_t());
            if(nullptr == m_pRMT_RemoteStrtErrRes.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_RemoteStrtErrRes.\n", __FUNCTION__);
                return false;
            }
            m_pRMT_RemoteStrtErrRes->operationSucceeded = res.m_operateSuccess;
            m_pRMT_RemoteStrtErrRes->errorCode = res.m_errorCode;
            m_pRMT_RemoteStrtErrRes->header = _heade;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, RMT_Climatization, UNKNOW m_resType, m_resType: %d", __FUNCTION__, res.m_resType);
            break;
        }


        /*switch (res.m_serviceType)
        {
        case RMT_Climatization:
            {
                switch (res.m_resType){
                    case ResType_StrtStop:
                    {
                        m_pRMT_ClimateStrtStopRes.reset(new fsm::RMT_ClimateSrtStopResult_t());
                        if (nullptr == m_pRMT_ClimateStrtStopRes.get()){
                            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_ClimateStrtStopRes.\n", __FUNCTION__);
                            return false;
                        }
                        if (vc::ErsStrtApplSts_ErsStsOff == res.m_engine_state){ // stop status
                            m_pRMT_ClimateStrtStopRes->engine_status = app_EngineStatus_engine_off;
                        } else if (vc::ErsStrtApplSts_ErsStsRunng == res.m_engine_state){ //runng status
                            m_pRMT_ClimateStrtStopRes->engine_status = app_EngineStatus_engine_running;
                        } else { //other status Noset/strtg
                            m_pRMT_ClimateStrtStopRes->engine_status = app_EngineStatus_unknown;
                        }
                        m_pRMT_ClimateStrtStopRes->operationSucceeded = res.m_operateSuccess;
                        m_pRMT_ClimateStrtStopRes->speed = res.m_speed;
                        m_pRMT_ClimateStrtStopRes->chargeHvSts = res.m_chargeHvSts;
                        m_pRMT_ClimateStrtStopRes->header = _heade;
                        break;
                    }
                    case ResType_AutoStop:
                    {
                        m_pRMT_ClimateAutoStopRes.reset(new fsm::RMT_CliamteAutoStopResult_t());
                        if (nullptr == m_pRMT_ClimateAutoStopRes.get()){
                            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_ClimateAutoStopRes.\n", __FUNCTION__);
                            return false;
                        }
                        m_pRMT_ClimateAutoStopRes->operationSucceeded = res.m_operateSuccess;
                        m_pRMT_ClimateAutoStopRes->preClimateActive = res.m_active;
                        m_pRMT_ClimateAutoStopRes->header = _heade;
                        break;
                    }
                    case ResType_Err:
                    {
                        m_pRMT_RemoteStrtErrRes.reset(new fsm::RMT_RemoteStartCommErrorResult_t());
                        if(nullptr == m_pRMT_RemoteStrtErrRes.get()){
                            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_RemoteStrtErrRes.\n", __FUNCTION__);
                            return false;
                        }
                        m_pRMT_RemoteStrtErrRes->operationSucceeded = res.m_operateSuccess;
                        m_pRMT_RemoteStrtErrRes->errorCode = res.m_errorCode;
                        m_pRMT_RemoteStrtErrRes->header = _heade;
                    }
                    default:
                        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, RMT_Climatization, UNKNOW m_resType", __FUNCTION__ );
                        break;
                }
            }
            break;
        case RMT_SEATHEAT:{
            switch (res.m_resType){
                case ResType_StrtStop:{
                        break;
                    break;
                }
                case ResType_AutoStop:{
                    break;
                }
                case ResType_Err:{
                    break;
                }
                default:
                    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, RMT_Climatization, UNKNOW m_resType", __FUNCTION__ );
                    break;
            }
            break;
        }
        case RMT_SEATVENTI:{
            //TODO
            break;
        }
        case RMT_STEERWHEELHEAT:{
            //TODO
            break;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, UNKNOW service type", __FUNCTION__ );
            break;
        }*/
        return true;
    }

    bool GlyVdsRceSignal::UnpackPayload(void* vdsServiceRequest)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s", __FUNCTION__ );
        RceSignalAdapt _rce_adaptor(vdsServiceRequest);
        std::shared_ptr<AppDataBase> appData = _rce_adaptor.UnpackService();
        m_pRMT_ClimatizationReq = std::dynamic_pointer_cast<fsm::RMT_CliamtizationRequest_t>(appData);
        if(nullptr == m_pRMT_ClimatizationReq.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "GlyVdsRceSignal::%s, nullptr == m_pRMT_ClimatizationReq", __FUNCTION__ );
            return false;
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->serviceId:%d. \n",
                        __FUNCTION__, m_pRMT_ClimatizationReq->serviceId);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->serviceCommand:%d. \n",
                        __FUNCTION__, m_pRMT_ClimatizationReq->serviceCommand);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->interval:%d. \n",
                        __FUNCTION__, m_pRMT_ClimatizationReq->interval);
        if (-1 != m_pRMT_ClimatizationReq->rec_temp){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->rec_temp:%d. \n",
                            __FUNCTION__, m_pRMT_ClimatizationReq->rec_temp);
        }
        if (-1 != m_pRMT_ClimatizationReq->rec_Level){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->rec_Level:%d. \n",
                            __FUNCTION__, m_pRMT_ClimatizationReq->rec_Level);
        }
        if (-1 != m_pRMT_ClimatizationReq->rec_heat){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->rec_heat:%d. \n",
                            __FUNCTION__, m_pRMT_ClimatizationReq->rec_heat);
        }
        if (-1 != m_pRMT_ClimatizationReq->rec_venti){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->rec_venti:%d. \n",
                            __FUNCTION__, m_pRMT_ClimatizationReq->rec_venti);
        }

        if (nullptr == m_pRMT_ClimatizationReq.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, nullptr == m_pRMT_ClimatizationReq", __FUNCTION__ );
            return false;
        }
        m_climateReq.reset(new RMT_ClimatizationRequest_t());
        if (nullptr == m_climateReq.get()){
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRceSignal::%s, nullptr == m_climateReq", __FUNCTION__ );
            return false;
        }
        if(kTerminateService == m_pRMT_ClimatizationReq->serviceCommand){ // stop
            m_climateReq->m_operateCmd = CMD_Stop;
        } else if (kStartService == m_pRMT_ClimatizationReq->serviceCommand){ //start 
            m_climateReq->m_operateCmd = CMD_Start;
            m_climateReq->m_duration_time = m_pRMT_ClimatizationReq->interval;
        } else {
            m_climateReq->m_operateCmd = CMD_NoReq;
        }
        m_climateReq->m_level = (vc::TelmClimaTSetHmiCmptmtTSpSpcl)m_pRMT_ClimatizationReq->rec_Level;
        if(-1 != m_pRMT_ClimatizationReq->rec_temp){ // remote climate
            m_climateReq->m_serviceType = RMT_Climatization;
            m_climateReq->m_telemClimaTemperature = m_pRMT_ClimatizationReq->rec_temp;
        }
        if (-1 != m_pRMT_ClimatizationReq->rec_heat){
            if (1 == m_pRMT_ClimatizationReq->rec_heat%10){//steer wheel heat
                m_climateReq->m_serviceType = RMT_STEERWHEELHEAT;
                m_climateReq->m_RceHeat = m_pRMT_ClimatizationReq->rec_heat;
                m_climateReq->m_duration_time = GEELY_STEERING_WHEEL_HEAT_TIME;
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload m_climateReq->m_serviceType:%d. \n",
                            __FUNCTION__, m_climateReq->m_serviceType);
            } else { // seat heat
                m_climateReq->m_serviceType = RMT_SEATHEAT;
                m_climateReq->m_RceHeat = m_pRMT_ClimatizationReq->rec_heat;
            }
        }
        if (-1 != m_pRMT_ClimatizationReq->rec_venti){
            m_climateReq->m_serviceType = RMT_SEATVENTI;
            m_climateReq->m_RceVenti = m_pRMT_ClimatizationReq->rec_venti;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload m_climateReq->m_serviceType:%d. \n",
                            __FUNCTION__, m_climateReq->m_serviceType);
        }
        return true;
    }
}