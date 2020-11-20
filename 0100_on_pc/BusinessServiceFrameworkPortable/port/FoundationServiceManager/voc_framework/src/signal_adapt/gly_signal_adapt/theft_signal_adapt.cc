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
#include "voc_framework/signal_adapt/theft_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

TheftSignalAdapt::TheftSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), memset m_vdsData begin  \n", __FUNCTION__);
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_DEBUG, "%s(), memset m_vdsData end  \n", __FUNCTION__);
}

bool TheftSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s\n", __FUNCTION__);

    if(vdsData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s vdsData == nullptr.\n", __FUNCTION__);
        return false;
    }

    appData = new TheftRequest_t();
    TheftRequest_t *callData = dynamic_cast<TheftRequest_t*>(appData);
    if(callData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s callData == nullptr.\n", __FUNCTION__);
        return false;
    }

    callData->serviceId = vdsData->body.serviceId;

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s completed.\n", __FUNCTION__);

    return true;
}

bool TheftSignalAdapt::SetResultPayload(const TheftServiceResult_t &response)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s\n", __FUNCTION__);

    VDServiceRequest_Model vdsData;
    memset(&vdsData, 0, sizeof(vdsData));

    vdsData.body_presence = true;
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_stn;
    vdsData.body.serviceData_presence = true;
    vdsData.body.serviceData.vehicleStatus_presence = true;
    vdsData.body.serviceData.vehicleStatus.theftNotification_presence = true;
    vdsData.body.serviceData.vehicleStatus.theftNotification.time.seconds = response.seconds;
    vdsData.body.serviceData.vehicleStatus.theftNotification.time.milliseconds_presence = true;
    vdsData.body.serviceData.vehicleStatus.theftNotification.time.milliseconds = response.milliseconds;
    vdsData.body.serviceData.vehicleStatus.theftNotification.activated = response.activated;

    bool result = PackService(vdsData, response);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "TheftSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

