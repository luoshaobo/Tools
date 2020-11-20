///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file signal_adapt_base.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include "timestamp.h"
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"

#include "voc_framework/signal_adapt/signal_adapt_base.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

SignalAdaptBase::SignalAdaptBase(void * vdsObj)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, 
        "%s(), set void * vdsObj to m_vdsObj \n", 
        __FUNCTION__);
    m_vdsObj = (asn_wrapper::VDServiceRequest *)vdsObj;
}

SignalAdaptBase::SignalAdaptBase(asn_wrapper::VDServiceRequest* vdsObj)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s(vdsObj)\n", __FUNCTION__);
    m_vdsObj = vdsObj;
}

 SignalAdaptBase::~SignalAdaptBase()
 {
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s\n", __FUNCTION__);
 }

std::shared_ptr<AppDataBase> SignalAdaptBase::UnpackService()
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s.\n", __FUNCTION__);
    
    AppDataBase *appData = NULL;
    std::shared_ptr<VDServiceRequest_Model> vdsData = std::make_shared<VDServiceRequest_Model>();
    
    VdsMarbenAdapt adapt(m_vdsObj);
    adapt.VDServiceRequest_Decode("", vdsData);
    
    GetBodyPayload(vdsData, appData);
    
    if(appData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s GetBodyPayload fail.\n", __FUNCTION__);
        return nullptr;
    }
    
    GetHeadPayload(vdsData, appData->header);

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s completed.\n", __FUNCTION__);

    return std::shared_ptr<AppDataBase>(appData);
}

bool SignalAdaptBase::PackService(VDServiceRequest_Model &vdsData, const AppDataBase &appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s.\n", __FUNCTION__);
    
    SetHeadPayload(vdsData, appData.header);

    VdsMarbenAdapt adapt(m_vdsObj);
    bool result = adapt.VDServiceRequest_Encode(vdsData);
    
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s VDServiceRequest_Encode error.\n", __FUNCTION__);
        return result;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s completed.\n", __FUNCTION__);

    return true;
}

bool SignalAdaptBase::GetHeadPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, RequestHeader_Data &appData)
{
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }
    
    appData.requestid = vdsData->header.requestid;
    appData.timeStampSeconds = vdsData->header.timeStamp.seconds;
    appData.timeStampMilliseconds_presence = vdsData->header.timeStamp.milliseconds_presence;
    appData.timeStampMilliseconds = vdsData->header.timeStamp.milliseconds;
    appData.eventId_presence = vdsData->header.eventId_presence;
    appData.eventIdSeconds = vdsData->header.eventId.seconds;
    appData.eventIdMilliseconds_presence = vdsData->header.eventId.milliseconds_presence;
    appData.eventIdMilliseconds = vdsData->header.eventId.milliseconds;
    appData.creatorId_presence = vdsData->header.creatorId_presence;
    appData.creatorId = vdsData->header.creatorId;
    appData.messageTTL_presence = vdsData->header.messageTTL_presence;
    appData.messageTTL = vdsData->header.messageTTL;
    appData.requestType_presence =vdsData->header.requestType_presence;
    appData.requestType = vdsData->header.requestType;
    appData.ackRequired_presence = vdsData->header.ackRequired_presence;
    appData.ackRequired = vdsData->header.ackRequired;
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s completed.\n", __FUNCTION__);
    
    return true;
}


bool SignalAdaptBase::SetHeadPayload(VDServiceRequest_Model &vdsData, const RequestHeader_Data &appData)
{
    Timestamp now;
    vdsData.header.timeStamp.seconds = (now.epochMicroseconds()/1000000);
    vdsData.header.timeStamp.milliseconds_presence = true;
    vdsData.header.timeStamp.milliseconds= (now.epochMicroseconds()%1000000)/1000;
    vdsData.header.creatorId_presence = true;
    vdsData.header.creatorId =(Vds_CreatorId)app_ecu;
    vdsData.header.messageTTL_presence = true;
    vdsData.header.messageTTL = 120;
    vdsData.header.requestType_presence = true;
    if(appData.requestType_presence){
        vdsData.header.requestType = (Vds_RequestType)appData.requestType;
    } else {
        vdsData.header.requestType = Vds_serviceRequest;
    }
    vdsData.header.ackRequired_presence = true;
    vdsData.header.ackRequired = false;
    vdsData.header.requestid = getRequestId();

    if(!appData.isInitiator)
    {
        vdsData.header.eventId_presence = appData.eventId_presence;
        vdsData.header.eventId.seconds = appData.eventIdSeconds;
        vdsData.header.eventId.milliseconds_presence = appData.eventIdMilliseconds_presence;
        vdsData.header.eventId.milliseconds= appData.eventIdMilliseconds;
    }
    else
    {
        vdsData.header.eventId_presence = true;
        vdsData.header.eventId.seconds = vdsData.header.timeStamp.seconds ;
        vdsData.header.eventId.milliseconds_presence = true;
        vdsData.header.eventId.milliseconds= vdsData.header.timeStamp.milliseconds;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SignalAdaptBase::%s completed.\n", __FUNCTION__);
    
    return true;
}

