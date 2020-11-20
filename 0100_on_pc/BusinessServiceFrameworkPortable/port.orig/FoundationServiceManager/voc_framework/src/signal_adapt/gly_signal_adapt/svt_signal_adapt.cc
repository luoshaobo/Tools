///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file svt_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/svt_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

SvtSignalAdapt::SvtSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj)
{}

bool SvtSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s\n", __FUNCTION__);
    
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new SvtRequest_t();
    SvtRequest_t *svtData = dynamic_cast<SvtRequest_t*>(appData);
    if(svtData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s svtData == nullptr.\n", __FUNCTION__);
        return false;
    }

    svtData->serviceId = vdsData->body.serviceId;
    svtData->serviceCommand = vdsData->body.serviceData.serviceCommand;
    if(svtData->serviceCommand != app_terminateService)
    {
        svtData->scheduledTimeSeconds = vdsData->body.serviceData.serviceTrigger.scheduledTime.seconds;
        svtData->scheduledTimeMilliseconds = vdsData->body.serviceData.serviceTrigger.scheduledTime.milliseconds;
        svtData->endTimeSeconds = vdsData->body.serviceData.serviceTrigger.endTime.seconds;
        svtData->endTimeMilliseconds = vdsData->body.serviceData.serviceTrigger.endTime.milliseconds;
        svtData->interval = (vdsData->body.serviceData.serviceTrigger.interval) * 10;//unit is 10s
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s completed.\n", __FUNCTION__);
    
    return true;
}

bool SvtSignalAdapt::SetResultPayload(const SvtServiceResult_t &response)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;
    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_svt;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;
    vdsData.body.serviceData.serviceResult.operationSucceeded = response.operationSucceeded;
    if(!response.operationSucceeded)
    {
        vdsData.body.serviceData.serviceResult.error_presence = true;
        vdsData.body.serviceData.serviceResult.error.code = (Vds_ErrorCode)response.errorCode;
    }
    
    bool result = PackService(vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

bool SvtSignalAdapt::SetTrackPointPayload(const SvtTrackPoint_t &response)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;
    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_svt;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.trackPoints_presence = true;

    TrackPoint_Model trackPoint;
    trackPoint.systemTime.seconds = response.systemTimeSeconds;
    trackPoint.systemTime.milliseconds_presence = true;
    trackPoint.systemTime.milliseconds_presence = response.systemTimeMilliseconds;
    trackPoint.isSVT_presence = true;
    trackPoint.isSVT = response.isSVT;
    trackPoint.speed = response.speed;
    trackPoint.position.latitude_presence = true;
    trackPoint.position.latitude = response.latitude;
    trackPoint.position.longitude_presence = true;
    trackPoint.position.longitude = response.longitude;
    trackPoint.position.altitude_presence = true;
    trackPoint.position.altitude = response.altitude;
    trackPoint.position.posCanBeTrusted_presence = true; 
    trackPoint.position.posCanBeTrusted = response.posCanBeTrusted;
    trackPoint.position.carLocatorStatUploadEn_presence = true;
    trackPoint.position.carLocatorStatUploadEn = response.carLocatorStatUploadEn;
    trackPoint.position.marsCoordinates_presence = true;
    trackPoint.position.marsCoordinates = response.marsCoordinates;
    vdsData.body.serviceData.trackPoints.push_back(trackPoint);

    bool result = PackService(vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "McuSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "SvtSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}
