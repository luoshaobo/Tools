///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file gly_vds_res_signal.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      uia93888
// @Init date   07-Sep-2018
///////////////////////////////////////////////////////////////////



#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_res_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/res_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

std::shared_ptr<fsm::Signal> GlyVdsResSignal::CreateGlyVdsResSignal(fsm::VdServiceTransactionId& transaction_id,void* vdsService)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.\n", __FUNCTION__);
    GlyVdsResSignal* signal = new GlyVdsResSignal(transaction_id, vdsService);
    
    return std::shared_ptr<GlyVdsResSignal>(signal);
}

GlyVdsResSignal::GlyVdsResSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceRes)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.\n", __FUNCTION__);
    bool result = UnpackPayload(vdsService);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.get res request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.get res request payload fail.\n", __FUNCTION__);
    }
}


GlyVdsResSignal::~GlyVdsResSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.\n", __FUNCTION__);
}

bool GlyVdsResSignal::UnpackPayload(void* vdsServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.\n", __FUNCTION__);
    ResSignalAdapt _res_adaptor(vdsServiceRequest);
    std::shared_ptr<AppDataBase> appData = _res_adaptor.UnpackService();
    m_pRMT_EngineReq = std::dynamic_pointer_cast<fsm::RMT_EngineRequest_t>(appData);

    if(nullptr == m_pRMT_EngineReq.get())
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineReq.\n", __FUNCTION__);
        return false;
    }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr != m_pRMT_EngineReq.\n", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->serviceId:%d. \n", __FUNCTION__, m_pRMT_EngineReq->serviceId);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->serviceCommand:%d. \n", __FUNCTION__, m_pRMT_EngineReq->serviceCommand);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UnpackPayload request->interval:%d. \n", __FUNCTION__, m_pRMT_EngineReq->interval);
    return true;
}


bool GlyVdsResSignal::SetResultPayload(ErsResponse_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s.\n", __FUNCTION__);
    RequestHeader_Data _heade;
    if(nullptr == m_pRMT_EngineReq.get()) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineReq.get().\n", __FUNCTION__);
        /*if (ErsResponseType_Notify == response.type_){
            //TODO
        } else {*/
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineReq.get().\n", __FUNCTION__);
            return false;
        //}
    } else {
        _heade = m_pRMT_EngineReq->header;
    }
    switch (response.type_)
    {
        case ErsResponseType_Start:
        {
            m_pRMT_EngineStrtRes.reset(new fsm::RMT_EngineStartResult_t());
            if(nullptr == m_pRMT_EngineStrtRes.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineStrtRes.\n", __FUNCTION__);
                return false;
            }
            m_pRMT_EngineStrtRes->operationSucceeded = response.operationSucceeded;
            if (vc::ErsStrtApplSts_ErsStsOff == response.engine_state){ // stop status
                m_pRMT_EngineStrtRes->engine_status = app_EngineStatus_engine_off;
            } else if (vc::ErsStrtApplSts_ErsStsRunng == response.engine_state){ //runng status
                m_pRMT_EngineStrtRes->engine_status = app_EngineStatus_engine_running;
            } else { //other status Noset/strtg
                m_pRMT_EngineStrtRes->engine_status = app_EngineStatus_unknown;
            }
            m_pRMT_EngineStrtRes->speed = response.speed;
            m_pRMT_EngineStrtRes->doorLockStatusPassenger = response.rmt_status.doorPassLockSts;
            m_pRMT_EngineStrtRes->doorOpenStatusPassenger = response.rmt_status.doorPassSts;
            m_pRMT_EngineStrtRes->doorLockStatusDriverRear = response.rmt_status.doorLeReLockSrs;
            m_pRMT_EngineStrtRes->doorOpenStatusDriverRear = response.rmt_status.doorLeReSts;
            m_pRMT_EngineStrtRes->doorLockStatusDriver = response.rmt_status.doorDrvrLockSts;
            m_pRMT_EngineStrtRes->doorOpenStatusDriver = response.rmt_status.doorDrvrSts;
            m_pRMT_EngineStrtRes->doorLockStatusPassengerRear = response.rmt_status.doorRiReLockSts;
            m_pRMT_EngineStrtRes->doorOpenStatusPassengerRear = response.rmt_status.doorRiReSts;
            m_pRMT_EngineStrtRes->trunkLockStatus = response.rmt_status.trLockSts;
            m_pRMT_EngineStrtRes->trunkOpenStatus = response.rmt_status.trSts;
            m_pRMT_EngineStrtRes->engineHoodOpenStatus = response.rmt_status.hoodSts;
            m_pRMT_EngineStrtRes->centralLockingStatus = response.rmt_status.lockgCenStsForUsrFb;
            m_pRMT_EngineStrtRes->engineOilLevelStatus = response.rmt_status.engOilLvlSts;
            m_pRMT_EngineStrtRes->electricParkBrakeStatus = response.rmt_status.epbSts;
            m_pRMT_EngineStrtRes->fuelLevel = response.rmt_status.fuLvlIndcd;
            m_pRMT_EngineStrtRes->fuelLevelStatus = response.rmt_status.fuLvlLoIndcn;
            m_pRMT_EngineStrtRes->header = _heade;
            break;
        }
        case ErsResponseType_Stop:
        {
            m_pRMT_EngineStopRes.reset(new fsm::RMT_EngineStopResult_t());
            if(nullptr == m_pRMT_EngineStopRes.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineStopRes.\n", __FUNCTION__);
                return false;
            }
            m_pRMT_EngineStopRes->operationSucceeded = response.operationSucceeded;
            if (vc::ErsStrtApplSts_ErsStsOff == response.engine_state){ // stop status
                m_pRMT_EngineStopRes->engine_status = app_EngineStatus_engine_off;
            } else if (vc::ErsStrtApplSts_ErsStsRunng == response.engine_state){ //runng status
                m_pRMT_EngineStopRes->engine_status = app_EngineStatus_engine_running;
            } else { //other status Noset/strtg
                m_pRMT_EngineStopRes->engine_status = app_EngineStatus_unknown;
            }
            m_pRMT_EngineStopRes->speed = response.speed;
            m_pRMT_EngineStopRes->usageMode = response.usgeMode_state;
            m_pRMT_EngineStopRes->header = _heade;
            break;
        }
        case ErsResponseType_Notify:
        {
            m_pRMT_EngineNotify.reset(new fsm::RMT_EngineNotify_t);
            if(nullptr == m_pRMT_EngineNotify.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineNotify.\n", __FUNCTION__);
                return false;
            }
            if (vc::ErsStrtApplSts_ErsStsOff == response.engine_state){ // stop status
                m_pRMT_EngineNotify->engine_status = app_EngineStatus_engine_off;
            } else if (vc::ErsStrtApplSts_ErsStsRunng == response.engine_state){ //runng status
                m_pRMT_EngineNotify->engine_status = app_EngineStatus_engine_running;
            } else { //other status Noset/strtg
                m_pRMT_EngineNotify->engine_status = app_EngineStatus_unknown;
            }
            m_pRMT_EngineNotify->speed = response.speed;
            m_pRMT_EngineNotify->header = _heade;
            break;
        }
        case ErsResponseType_Error:
        {
            m_pRMT_EngineErrRes.reset(new fsm::RMT_EngineErrorResult_t);
            if(nullptr == m_pRMT_EngineErrRes.get()){
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, nullptr == m_pRMT_EngineErrRes.\n", __FUNCTION__);
                return false;
            }
            m_pRMT_EngineErrRes->operationSucceeded = response.operationSucceeded;
            m_pRMT_EngineErrRes->errorCode = response.errorCode;
            m_pRMT_EngineErrRes->vehicleErrorCode = response.vehicleErrorCode;
            m_pRMT_EngineErrRes->message = response.message;
            m_pRMT_EngineErrRes->header = _heade;
            break;
        }
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, UNKNOW ErsResponseType: %d\n", __FUNCTION__, response.type_);
            break;
        }
        return true;
    }


std::shared_ptr<ErsRequest_t> GlyVdsResSignal::GetRequestPayload()
{
    if (nullptr == m_pRMT_EngineReq.get()){
        return nullptr;
    }
    m_pErsRequest.reset(new ErsRequest_t);
    if(kTerminateService == m_pRMT_EngineReq->serviceCommand){
        m_pErsRequest->ersCmd = vc::ErsCmd_ErsCmdOff;
        m_pErsRequest->ersRunTime = RMT_ENGINE_START_DEFAULT_TIME;
    } else if (kStartService == m_pRMT_EngineReq->serviceCommand){
        m_pErsRequest->ersCmd = vc::ErsCmd_ErsCmdOn;
        if ((RMT_ENGINE_START_MIX_TIME < (m_pErsRequest->ersRunTime = (m_pRMT_EngineReq->interval)/60)) ||
            (RMT_ENGINE_START_MIN_TIME >= (m_pErsRequest->ersRunTime = (m_pRMT_EngineReq->interval)/60))){
            m_pErsRequest->ersRunTime = RMT_ENGINE_START_DEFAULT_TIME;
        } else {
            m_pErsRequest->ersRunTime = (m_pRMT_EngineReq->interval)/60;
        }
    } else {
        m_pErsRequest->ersCmd = vc::ErsCmd_ErsCmdNotSet;
    }
    return m_pErsRequest;
}


/*bool GlyVdsResSignal::SetHeadPayload()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsSvtSignal::%s\n", __FUNCTION__);
    
    if (m_pVdServiceResponse)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s free_struct.m_pVdServiceResponse addr:%0x\n", __FUNCTION__, m_pVdServiceResponse);
        delete m_pVdServiceResponse;
    }
    m_pVdServiceResponse = new VDServiceRequest();
    VdmSignal::SetHeadPayload(m_pVdServiceResponse);
    if(m_pVdServiceRequest == nullptr ||
       m_pVdServiceRequest->header == nullptr ||
       m_pVdServiceRequest->header->eventId == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s m_pVdServiceRequest->header->eventId == nullptr.\n", __FUNCTION__);
        return false;
    }
    else
    {
        m_pVdServiceResponse->header->optional.setPresence(asn_eventId);
        m_pVdServiceResponse->header->eventId = new TimeStamp();
        
        m_pVdServiceResponse->header->eventId->seconds = m_pVdServiceRequest->header->eventId->seconds;
        
        m_pVdServiceResponse->header->eventId->optional.setPresence(asn_milliseconds);
        m_pVdServiceResponse->header->eventId->milliseconds= m_pVdServiceRequest->header->eventId->milliseconds;
    }

    return true;
}*/

vc::ErsCmd GlyVdsResSignal::GetOperationCommand()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s\n", __FUNCTION__);
    if(kStartService == m_pRMT_EngineReq->serviceCommand) { //start engine
        return vc::ErsCmd_ErsCmdOn;
    } else if (kTerminateService == m_pRMT_EngineReq->serviceCommand){ //stop engine
        return vc::ErsCmd_ErsCmdOff;
    } else {
        return vc::ErsCmd_ErsCmdNotSet;
    }
}

int GlyVdsResSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s\n", __FUNCTION__);
    if(NULL == vdServiceRequest){
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsResSignal::%s, NULL == vdServiceRequest\n", __FUNCTION__);
            if (nullptr != m_pRMT_EngineNotify){
            ResSignalAdapt _res_adaptor((void*)vdServiceRequest);
            bool ret = _res_adaptor.SetNotifyPayload(*m_pRMT_EngineNotify);
            return (ret == true? 0 : -1);
        } else {
            return -1;
        }
    }
    if(nullptr != m_pRMT_EngineStrtRes){
        ResSignalAdapt _res_adaptor((void*)vdServiceRequest);
        bool ret = _res_adaptor.SetStartResultPayload(*m_pRMT_EngineStrtRes);
        return (ret == true? 0 : -1);
    } else if (nullptr != m_pRMT_EngineStopRes){
        ResSignalAdapt _res_adaptor((void*)vdServiceRequest);
        bool ret = _res_adaptor.SetStopResultPayload(*m_pRMT_EngineStopRes);
        return (ret == true? 0 : -1);
    } else if (nullptr != m_pRMT_EngineErrRes){
        ResSignalAdapt _res_adaptor((void*)vdServiceRequest);
        bool ret = _res_adaptor.SetErrorResultPayload(*m_pRMT_EngineErrRes);
        return (ret == true? 0 : -1);
    } else if (nullptr != m_pRMT_EngineNotify){
        ResSignalAdapt _res_adaptor((void*)vdServiceRequest);
        bool ret = _res_adaptor.SetNotifyPayload(*m_pRMT_EngineNotify);
        return (ret == true? 0 : -1);
    }
    return -1;
}


} // namespace volvo_on_call
// }    end of addtogroup 
