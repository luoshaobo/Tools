///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rmc_signal_adapt.cc

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Nie Yujin
// @Init date	13-Mar-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/rmc_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);
namespace fsm
{
RemoteCtrlBasicRequest::RemoteCtrlBasicRequest()
    :serviceId(0)
    ,serviceCommand(0)
    ,timeStart(0)
    ,timeWindow(0)
    ,requestType(0)
    ,doorValue(0x000000ff)
    ,targetValue(0)
    ,value(-1)
    ,pos(-1)
{
}
RemoteCtrlBasicRequest::~RemoteCtrlBasicRequest()
{
}
RemoteCtrlPosition::RemoteCtrlPosition()
    :latitude(0)
    ,longitude(0)
    ,altitude(0)
    ,posCanBeTrusted(0)
    ,carLocatorStatUploadEn(0)
    ,marsCoordinates(0)
{
}
RemoteCtrlPosition::~RemoteCtrlPosition()
{
}
RemoteCtrlBasicStatus::RemoteCtrlBasicStatus()
    :serviceId(0)
    ,position()
    ,vehSpdIndcd(0)
    ,vehSpdIndcdQly(0)
    ,doorOpenStatusDriver(0)
    ,doorOpenStatusPassenger(0)
    ,doorOpenStatusDriverRear(0)
    ,doorOpenStatusPassengerRear(0)
    ,doorLockStatusDriver(0)
    ,doorLockStatusPassenger(0)
    ,doorLockStatusDriverRear(0)
    ,doorLockStatusPassengerRear(0)
    ,trunkOpenStatus(0)
    ,trunkLockStatus(0)
    ,ventilateStatus(0)
    ,hashoodLockSts(false)
    ,centralLockingStatus(0)
    ,hoodSts(0)
    ,hoodLockSts(0)
    ,lockgCenStsForUsrFb(0)
    ,winStatusDriver(0)
    ,winStatusPassenger(0)
    ,winStatusDriverRear(0)
    ,winStatusPassengerRear(0)
    ,winStatusDriverWarning(0)
    ,winStatusPassengerWarning(0)
    ,winStatusDriverRearWarning(0)
    ,winStatusPassengerRearWarning(0)
    ,sunroofOpenStatus(0)
    ,curtainOpenStatus(0)
    ,sunroofPos(0)
    ,curtainPos(0)
    ,winPosDriver(0)
    ,winPosPassenger(0)
    ,winPosDriverRear(0)
    ,winPosPassengerRear(0)

    ,interiorPM25(0)
    ,exteriorPM25(0)
    ,interiorPM25Level(0)
    ,exteriorPM25Level(0)
{
}
RemoteCtrlBasicStatus::~RemoteCtrlBasicStatus()
{
}
RemoteCtrlBasicResult::RemoteCtrlBasicResult()
    :serviceId(0)
    ,operationSucceeded(0)
    ,errorCode(0)
    ,vehicleErrorCode(0)
    ,message()
{
}
RemoteCtrlBasicResult::~RemoteCtrlBasicResult()
{
}

bool RmcSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s\n", __FUNCTION__);
    
    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new RemoteCtrlBasicRequest();
    RemoteCtrlBasicRequest *request = dynamic_cast<RemoteCtrlBasicRequest*>(appData);
    if(request == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s request == nullptr.\n", __FUNCTION__);
        return false;
    }
    request->serviceId = vdsData->body.serviceId;
    request->serviceCommand = vdsData->body.serviceData.serviceCommand;

    for (auto pServiceParameter = vdsData->body.serviceData.serviceParameters.begin(); pServiceParameter != vdsData->body.serviceData.serviceParameters.end();) 
    {
        if (app_rdl == request->serviceId)
        {
            request->requestType |= RMC_RDL_DOOR;
            if (0 == pServiceParameter->key.compare("rdl.door"))
            {
                request->doorValue = pServiceParameter->intVal;
            }
            if (0 == pServiceParameter->key.compare("rdl.target"))
            {
                request->targetValue = pServiceParameter->intVal;
                request->requestType |= RMC_RDL_TARGET;
            }
        }
        if (app_rdu == request->serviceId)
        {
            request->requestType |= RMC_RDU_DOOR;
            if (0 == pServiceParameter->key.compare("rdu.door"))
            {
                request->doorValue = pServiceParameter->intVal;
            }
            if (0 == pServiceParameter->key.compare("rdu.target"))
            {
                request->targetValue = pServiceParameter->intVal;
                request->requestType |= RMC_RDU_TARGET;
            }
        }

        if (0 == pServiceParameter->key.compare("time.start"))
        {
            request->timeStart = pServiceParameter->timestampVal.seconds;
        }
        if (0 == pServiceParameter->key.compare("time.window"))
        {
            request->timeWindow = pServiceParameter->intVal;
        }
#ifdef RDO_RDC
        if (app_rdo == request->serviceId)
        {
            if (0 == pServiceParameter->key.compare("rdo.door"))
            {
                request->doorValue = pServiceParameter->intVal;
            }
            if (0 == pServiceParameter->key.compare("rdo.target"))
            {
                request->targetValue = pServiceParameter->intVal;
            }
        }
        if (app_rdc == request->serviceId)
        {
            if (0 == pServiceParameter->key.compare("rdc.door"))
            {
                request->doorValue = pServiceParameter->intVal;
            }
            if (0 == pServiceParameter->key.compare("rdc.target"))
            {
                request->targetValue = pServiceParameter->intVal;
            }
        }
#endif
        if (0 == pServiceParameter->key.compare("rhl"))
        {
            request->value = pServiceParameter->intVal;
        }
        if (app_rws == request->serviceId)
        {
            if (0 == pServiceParameter->key.compare("rws"))
            {
                request->value = pServiceParameter->intVal;
            }
            if (0 == pServiceParameter->key.compare("Rws.pos"))
            {
                request->pos = pServiceParameter->intVal;
            }
        }

        ++pServiceParameter;
        
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s completed.\n", __FUNCTION__);
    
    return true;
}

bool RmcSignalAdapt::SetResultPayload(const RemoteCtrlBasicResult &result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = static_cast<Vds_ServiceId>(result.serviceId);
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.serviceResult_presence = true;

    SetServiceResult(result, vdsData.body.serviceData.serviceResult);
    bool ret = PackService(vdsData, result);
    if(!ret)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return ret;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s completed.\n", __FUNCTION__);
    return ret;
}

bool RmcSignalAdapt::SetDoorsStatus(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = static_cast<Vds_ServiceId>(response.serviceId);
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus_presence = true;

    vdsData.body.serviceData.serviceResult_presence = true;
    SetServiceResult(result, vdsData.body.serviceData.serviceResult);

    SetDoorOpenStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetDoorLockStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetHoodStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetTrunkStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetCentralLockStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    bool ret = PackService(vdsData, response);
    if(!ret)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return ret;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s completed.\n", __FUNCTION__);
    return ret;
}

bool RmcSignalAdapt::SetWinStatus(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = static_cast<Vds_ServiceId>(response.serviceId);
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.lockStatus_presence = true;

    vdsData.body.serviceData.serviceResult_presence = true;
    SetServiceResult(result, vdsData.body.serviceData.serviceResult);

    SetWinStatusStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetWinPosStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetSunroofStsPos(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetCurtainStsPos(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    SetVentilateStatus(response, vdsData.body.serviceData.vehicleStatus.lockStatus);
    bool ret = PackService(vdsData, response);
    if(!ret)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return ret;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s completed.\n", __FUNCTION__);
    return ret;
}

bool RmcSignalAdapt::SetPM25Status(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = static_cast<Vds_ServiceId>(response.serviceId);
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.pollutionStatus_presence= true;

    vdsData.body.serviceData.serviceResult_presence = true;
    SetServiceResult(result, vdsData.body.serviceData.serviceResult);

    SetPollutionStatus(response, vdsData.body.serviceData.vehicleStatus.pollutionStatus);
    bool ret = PackService(vdsData, response);
    if(!ret)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return ret;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s completed.\n", __FUNCTION__);
    return ret;
}

bool RmcSignalAdapt::SetHALStatus(const RemoteCtrlBasicStatus &response, const RemoteCtrlBasicResult &result)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s\n", __FUNCTION__);
    
    VDServiceRequest_Model vdsData;

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = static_cast<Vds_ServiceId>(response.serviceId);
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.basicVehicleStatus_presence= true;

    vdsData.body.serviceData.serviceResult_presence = true;
    SetServiceResult(result, vdsData.body.serviceData.serviceResult);

    SetSpeedStatus(response, vdsData.body.serviceData.vehicleStatus.basicVehicleStatus);
    SetPositionStatus(response.position, vdsData.body.serviceData.vehicleStatus.basicVehicleStatus.position);
    bool ret = PackService(vdsData, response);
    if(!ret)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return ret;
    }
    
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s completed.\n", __FUNCTION__);
    return ret;
}

bool RmcSignalAdapt::SetServiceResult(const RemoteCtrlBasicResult &appData, ServiceResult_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;

    vdsData.operationSucceeded = appData.operationSucceeded;

    if(!appData.operationSucceeded)
    {   
        vdsData.error_presence = true;
        vdsData.error.code = static_cast<Vds_ErrorCode>(appData.errorCode);
        if (!appData.message.empty())
        {
            vdsData.error.message_presence = true;
            vdsData.error.message = appData.message;
        }
    }
    return result;
}
bool RmcSignalAdapt::SetDoorOpenStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.doorOpenStatusDriver_presence = true;
    vdsData.doorOpenStatusDriver = appData.doorOpenStatusDriver;
    vdsData.doorOpenStatusPassenger_presence = true;
    vdsData.doorOpenStatusPassenger = appData.doorOpenStatusPassenger;
    vdsData.doorOpenStatusDriverRear_presence = true;
    vdsData.doorOpenStatusDriverRear = appData.doorOpenStatusDriverRear;
    vdsData.doorOpenStatusPassengerRear_presence = true;
    vdsData.doorOpenStatusPassengerRear = appData.doorOpenStatusPassengerRear;
    return result;
}
bool RmcSignalAdapt::SetDoorLockStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.doorLockStatusDriver_presence = true;
    vdsData.doorLockStatusDriver = appData.doorLockStatusDriver;
    vdsData.doorLockStatusPassenger_presence = true;
    vdsData.doorLockStatusPassenger = appData.doorLockStatusPassenger;
    vdsData.doorLockStatusDriverRear_presence = true;
    vdsData.doorLockStatusDriverRear = appData.doorLockStatusDriverRear;
    vdsData.doorLockStatusPassengerRear_presence = true;
    vdsData.doorLockStatusPassengerRear = appData.doorLockStatusPassengerRear;
    return result;
}
bool RmcSignalAdapt::SetHoodStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.engineHoodOpenStatus_presence = true;
    vdsData.engineHoodOpenStatus = appData.hoodSts;
    vdsData.engineHoodLockStatus_presence = appData.hashoodLockSts;
    vdsData.engineHoodLockStatus = appData.hoodLockSts;
    return result;
}
bool RmcSignalAdapt::SetTrunkStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.trunkOpenStatus_presence = true;
    vdsData.trunkOpenStatus = appData.trunkOpenStatus;
    vdsData.trunkLockStatus_presence = true;
    //vdsData.trunkLockStatus = appData.trunkLockStatus;
    //temp code for GLY-3674
    //value 1 map to 0 in VDS
    //value 2 map to 1 in VDS
    //other values not support, customer will update it.
    if (1 == appData.trunkLockStatus)
    {
        vdsData.trunkLockStatus = 0;
    }
    else if (2 == appData.trunkLockStatus)
    {
        vdsData.trunkLockStatus = 1;
    }
    else
    {
        vdsData.trunkLockStatus_presence = false;
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s, trunkLockStatus(%d) is out of range", __FUNCTION__, appData.trunkLockStatus);
    }
    return result;
}
bool RmcSignalAdapt::SetCentralLockStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.centralLockingStatus_presence = true;
    vdsData.centralLockingStatus = appData.lockgCenStsForUsrFb;
    return result;
}
bool RmcSignalAdapt::SetWinStatusStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.winStatusDriver_presence = true;
    vdsData.winStatusDriver = appData.winStatusDriver;
    vdsData.winStatusPassenger_presence = true;
    vdsData.winStatusPassenger = appData.winStatusPassenger;
    vdsData.winStatusDriverRear_presence = true;
    vdsData.winStatusDriverRear = appData.winStatusDriverRear;
    vdsData.winStatusPassengerRear_presence = true;
    vdsData.winStatusPassengerRear = appData.winStatusPassengerRear;
    return result;
}
bool RmcSignalAdapt::SetWinWarningStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.winStatusDriverWarning_presence = true;
    vdsData.winStatusDriverWarning = appData.winStatusDriverWarning;
    vdsData.winStatusPassengerWarning_presence = true;
    vdsData.winStatusPassengerWarning = appData.winStatusPassengerWarning;
    vdsData.winStatusDriverRearWarning_presence = true;
    vdsData.winStatusDriverRearWarning = appData.winStatusDriverRearWarning;
    vdsData.winStatusPassengerRearWarning_presence = true;
    vdsData.winStatusPassengerRearWarning = appData.winStatusPassengerRearWarning;
    return result;
}
bool RmcSignalAdapt::SetWinPosStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.winPosDriver_presence = true;
    vdsData.winPosDriver = appData.winPosDriver;
    vdsData.winPosPassenger_presence = true;
    vdsData.winPosPassenger = appData.winPosPassenger;
    vdsData.winPosDriverRear_presence = true;
    vdsData.winPosDriverRear = appData.winPosDriverRear;
    vdsData.winPosPassengerRear_presence = true;
    vdsData.winPosPassengerRear = appData.winPosPassengerRear;
    return result;
}
bool RmcSignalAdapt::SetSunroofStsPos(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.sunroofOpenStatus_presence = true;
    vdsData.sunroofOpenStatus = appData.sunroofOpenStatus;
    vdsData.sunroofPos_presence = true;
    vdsData.sunroofPos = appData.sunroofPos;    
    return result;
}
bool RmcSignalAdapt::SetCurtainStsPos(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.curtainOpenStatus_presence = true;
    vdsData.curtainOpenStatus = appData.curtainOpenStatus;
    vdsData.curtainPos_presence = true;
    vdsData.curtainPos = appData.curtainPos;

    return result;
}
bool RmcSignalAdapt::SetVentilateStatus(const RemoteCtrlBasicStatus &appData, LockStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.ventilateStatus_presence = true;
    vdsData.ventilateStatus = appData.ventilateStatus;
    return result;
}
bool RmcSignalAdapt::SetPollutionStatus(const RemoteCtrlBasicStatus &appData, PollutionStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.interiorPM25_presence = true;
    vdsData.interiorPM25 = appData.interiorPM25;
    vdsData.exteriorPM25_presence = true;
    vdsData.exteriorPM25 = appData.exteriorPM25;
    vdsData.interiorPM25Level_presence = true;
    vdsData.interiorPM25Level = appData.interiorPM25Level;
    vdsData.exteriorPM25Level_presence = true;
    vdsData.exteriorPM25Level = appData.exteriorPM25Level;
    return result;
}
bool RmcSignalAdapt::SetSpeedStatus(const RemoteCtrlBasicStatus &appData, BasicVehicleStatus_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.speed = appData.vehSpdIndcd;
    vdsData.speedValidity_presence = true;
    vdsData.speedValidity = appData.vehSpdIndcdQly;
    return result;
}
bool RmcSignalAdapt::SetPositionStatus(const RemoteCtrlPosition &appData, Position_Model &vdsData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "RmcSignalAdapt::%s.", __FUNCTION__);
    bool result = true;
    vdsData.latitude_presence = true;
    vdsData.latitude = appData.latitude;
    vdsData.longitude_presence = true;
    vdsData.longitude = appData.longitude;
    vdsData.altitude = appData.altitude;
    vdsData.posCanBeTrusted_presence = true;
    vdsData.posCanBeTrusted = appData.posCanBeTrusted;
    vdsData.carLocatorStatUploadEn_presence = true;
    vdsData.carLocatorStatUploadEn = appData.carLocatorStatUploadEn;
    vdsData.marsCoordinates_presence = true;
    vdsData.marsCoordinates = appData.marsCoordinates;
    return result;
}
}

