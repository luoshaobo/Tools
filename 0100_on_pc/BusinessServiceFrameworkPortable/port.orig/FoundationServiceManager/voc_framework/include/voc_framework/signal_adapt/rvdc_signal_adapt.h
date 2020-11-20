///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rvdc_signal_adapt.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Nie Yujin
// @Init date	26-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef RVDC_SIGNAL_ADAPT_H
#define RVDC_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"
namespace fsm
{
class RvdcSignalAdapt: public SignalAdaptBase
{

protected:
    ////////////////////////////////////////////////////////////
    // @brief :unpack rvdc RequestBody. 
    // @return     True if successfully set, false otherwise
    // @author     nieyujin, 26-Feb-2019
    ////////////////////////////////////////////////////////////
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;
    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     nieyujin, 11-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetHeadPayload(VDServiceRequest_Model &vdsData, const RequestHeader_Data &appData) override;

public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return     
    // @author     nieyujin, 26-Feb-2019
    ////////////////////////////////////////////////////////////
    RvdcSignalAdapt(asn_wrapper::VDServiceRequest* vdsObj) : SignalAdaptBase(vdsObj){};
    RvdcSignalAdapt(void * vdsObj) : SignalAdaptBase(vdsObj){};
    bool SetRvdcPayload(const RVDC_Data &payload);
private:
    bool GetServiceParameters(std::vector<ServiceParameter_Data> &appData, const std::vector<ServiceParameter_Model> &vdsData);
    bool GetTimeStamp(TimeStamp_Data &appData, const TimeStamp_Model &vdsData);
    bool GetAuthorizationStatus(AuthorizationStatus_Data &appData, const AuthorizationStatus_Model &vdsData);
    bool GetMaSetting(MaSetting_Data &appData, const MaSetting_Model &vdsData);
    bool GetRvdcVehicleSetting(RvdcVehicleSetting_Data &appData, const RvdcVehicleSetting_Model &vdsData);
    bool GetLanguageSetting(LanguageSetting_Data &appData, const LanguageSetting_Model &vdsData);
    bool GetRvdcForcedSetting(RvdcForcedSetting_Data &appData, const RvdcForcedSetting_Model &vdsData);
    bool GetTelematicsParameters(TelematicsParameters_Data &appData, const TelematicsParameters_Model &vdsData);
    bool GetStopCondition(StopCondition_Data &appData, const StopCondition_Model &vdsData);
    bool GetUDS(UDS_Data &appData, const UDS_Model &vdsData);
    bool GetTrigger(Trigger_Data &appData, const Trigger_Model &vdsData);
    bool GetExecutionTrigger(ExecutionTrigger_Data &appData, const ExecutionTrigger_Model &vdsData);
    bool GetMa(Ma_Data &appData, const Ma_Model &vdsData);
    bool GetMdp(Mdp_Data &appData, const Mdp_Model &vdsData);
    bool GetRvdcSetting(RvdcSetting_Data &appData, const RvdcSetting_Model &vdsData);
    bool GetRvdcAssignmentNotification(RvdcAssignmentNotification_Data &appData, const RvdcAssignmentNotification_Model &vdsData);
    bool GetMaSynchronizationReq(MaSynchronizationReq_Data &appData, const MaSynchronizationReq_Model &vdsData);
    bool GetMaSynchronization(MaSynchronization_Data &appData, const MaSynchronization_Model &vdsData);
    bool GetRvdcExceptionHandling(RvdcExceptionHandling_Data &appData, const RvdcExceptionHandling_Model &vdsData);
    bool GetMeasurementDataPackage(MeasurementDataPackage_Data &appData, const MeasurementDataPackage_Model &vdsData);
    bool GetRVDC(RVDC_Data &appData, const RVDC_Model &vdsData);
    bool SetServiceParameters(const std::vector<ServiceParameter_Data> &appData, std::vector<ServiceParameter_Model> &vdsData);
    bool SetAuthorizationStatus(const AuthorizationStatus_Data &appData, AuthorizationStatus_Model &vdsData);
    bool SetMaSetting(const MaSetting_Data &appData, MaSetting_Model &vdsData);
    bool SetRvdcVehicleSetting(const RvdcVehicleSetting_Data &appData, RvdcVehicleSetting_Model &vdsData);
    bool SetLanguageSetting(const LanguageSetting_Data &appData, LanguageSetting_Model &vdsData);
    bool SetRvdcForcedSetting(const RvdcForcedSetting_Data &appData, RvdcForcedSetting_Model &vdsData);
    bool SetRvdcSetting(const RvdcSetting_Data &appData, RvdcSetting_Model &vdsData);
    bool SetTimeStamp(const TimeStamp_Data &appData, TimeStamp_Model &vdsData);
    bool SetTelematicsParameters(const TelematicsParameters_Data &appData, TelematicsParameters_Model &vdsData);
    bool SetMa(const Ma_Data &appData, Ma_Model &vdsData);
    bool SetMdp(const Mdp_Data &appData, Mdp_Model &vdsData);
    bool SetRvdcAssignmentNotification(const RvdcAssignmentNotification_Data &appData, RvdcAssignmentNotification_Model &vdsData);
    bool SetMaSynchronizationReq(const MaSynchronizationReq_Data &appData, MaSynchronizationReq_Model &vdsData);
    bool SetMeasurementDataPackage(const MeasurementDataPackage_Data &appData, MeasurementDataPackage_Model &vdsData);
    bool SetRVDC(const RVDC_Data &appData, RVDC_Model &vdsData);
};
}
#endif

