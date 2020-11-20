// Copyright (C) 2019 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @project     GLY_TCAM
// @subsystem   FSM
// @author      TanChang
// @Init date   27-Feb-2019

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/call_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

CallSignalAdapt::CallSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), memset m_vdsData begin  \n", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), memset m_vdsData end  \n", __FUNCTION__);
}

bool CallSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::%s\n", __FUNCTION__);

    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new CallRequest_t();
    CallRequest_t *callData = dynamic_cast<CallRequest_t*>(appData);
    if(callData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "callSignalAdapt::%s callData == nullptr.\n", __FUNCTION__);
        return false;
    }

    callData->serviceId = vdsData->body.serviceId;
    callData->serviceCommand = vdsData->body.serviceData.serviceCommand;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::SERVICE_COMMAND %d %s\n", callData->serviceCommand, __FUNCTION__);

    if(vdsData->body.serviceData_presence == true && vdsData->body.serviceData.serviceParameters_presence)
    {
        for (auto item = vdsData->body.serviceData.serviceParameters.begin(); item != vdsData->body.serviceData.serviceParameters.end(); ++item) 
        {
            if(item->timestampVal_presence == true)
            {
                callData->etavalue.push_back(item->timestampVal.seconds);
            }
        }
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::%s completed.\n", __FUNCTION__);

    return true;
}

bool CallSignalAdapt::SetResultPayload(const CallServiceResult_t &response)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::%s\n", __FUNCTION__);

    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_cec;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.automaticActivation = response.automaticActivation;
    vdsData.body.serviceData.vehicleStatus.msd.testCall = response.testCall;
    vdsData.body.serviceData.vehicleStatus.msd.fuelType = (Vds_FuelType)response.fuelType;
    vdsData.body.serviceData.vehicleStatus.msd.direction = response.direction;
    vdsData.body.serviceData.vehicleStatus.msd.position.latitude_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.position.longitude_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.position.altitude_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.position.posCanBeTrusted_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.position.carLocatorStatUploadEn_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.position.marsCoordinates_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.position.latitude = response.latitude;
    vdsData.body.serviceData.vehicleStatus.msd.position.longitude = response.longitude;
    vdsData.body.serviceData.vehicleStatus.msd.position.altitude = response.altitude;
    vdsData.body.serviceData.vehicleStatus.msd.position.posCanBeTrusted = response.posCanBeTrusted;
    vdsData.body.serviceData.vehicleStatus.msd.position.carLocatorStatUploadEn = response.carLocatorStatUploadEn;
    vdsData.body.serviceData.vehicleStatus.msd.position.marsCoordinates = response.marsCoordinates;
    vdsData.body.serviceData.vehicleStatus.msd.vin_presence = true;
    vdsData.body.serviceData.vehicleStatus.msd.vin = response.vin;

    bool result = PackService(vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "CallSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

