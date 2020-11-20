///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file mcu_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Li Jianhui
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_cus.h"
#include "vds_data_gen.h"
#include "vds_marben_adapt.h"
#include "voc_framework/signal_adapt/mta_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_libfsm);

bool MtaSignalAdapt::GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *&appData)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "MtaSignalAdapt::%s\n", __FUNCTION__);
    return false;
}

bool MtaSignalAdapt::SetMtaPayload(const fsm::MTAData_t &data)
{
    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "MtaSignalAdapt::%s\n", __FUNCTION__);

    VDServiceRequest_Model vdsData = {};

    vdsData.body_presence = true;

    // body.serviceId
    vdsData.body.serviceId_presence = true;
    vdsData.body.serviceId = (Vds_ServiceId)app_mta;

    // body.serviceData
    vdsData.body.serviceData_presence = true;

    // body.serviceData.vehicleStatus
    vdsData.body.serviceData.vehicleStatus_presence = true;

    // body.serviceData.vehicleStatus.temStatus
    vdsData.body.serviceData.vehicleStatus.temStatus_presence = true;
    TEMStatus_Model& temStatus = vdsData.body.serviceData.vehicleStatus.temStatus;

    if(!data.vin.empty())
    {
        temStatus.vin_presence = true; // vin
        temStatus.vin = data.vin;
    }

    if(!data.ecu_serial_number.empty())
    {
        temStatus.serialNumber_presence = true;// ecu_serial_number
        temStatus.serialNumber = data.ecu_serial_number;
    }

    // power source
    temStatus.powerSource = static_cast<Vds_PowerSource>(data.power_source);

    // power mode
    temStatus.powerMode = static_cast<Vds_TEMPowerMode>(data.power_mode);

    // networkAccessStatus, body.serviceData.vehicleStatus.temStatus.networkAccessStatus
    temStatus.networkAccessStatus_presence = true;
    NetworkAccessStatus_Model& networkAccessStatus = temStatus.networkAccessStatus;

    if(!data.mobile_network.empty())
    {
        networkAccessStatus.mobileNetwork_presence = true; // mobile_network
        networkAccessStatus.mobileNetwork = data.mobile_network;
    }
    //simInfo, body.serviceData.vehicleStatus.temStatus.networkAccessStatus.simInfo
    networkAccessStatus.simInfo_presence = true;
    SIMInfo_Model& simInfo = networkAccessStatus.simInfo;

    // msisdn
    if(!data.msisdn.empty())
    {
        simInfo.msisdn_presence = true;
        simInfo.msisdn = data.msisdn;
    }

    // imsi
    if(!data.imsi.empty())
    {
        simInfo.imsi_presence = true;
        simInfo.imsi = data.imsi;
    }

    // iccid
    if(!data.icc_id.empty())
    {
        simInfo.iccId_presence = true;
        simInfo.iccId = data.icc_id;
    }

    bool result = PackService(vdsData, data);
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "MtaSignalAdapt::%s PackService failed.\n", __FUNCTION__);
        return result;
    }

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "MtaSignalAdapt::%s completed.\n", __FUNCTION__);
    return result;
}

