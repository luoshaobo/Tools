
#ifndef RVS_SIGNAL_ADAPT_H
#define RVS_SIGNAL_ADAPT_H

#include <string>
#include <vector>
#include <memory>
#include "app_data.h"
#include "signal_adapt_base.h"
namespace fsm
{
class RvsSignalAdapt: public SignalAdaptBase
{

public:
    bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase * &appData) override;
    bool UnpackServiceParameters(RvsRequest_t *rvsData,
            std::shared_ptr<VDServiceRequest_Model> vdsData);
    bool UnpackEventRule(RvsRequest_t *rvsData,
            std::shared_ptr<VDServiceRequest_Model> vdsData);
    bool SetBodyPayload();
    bool checkFieldsInAsnScope_long(long inputValue, long minValue, long maxValue);
    bool checkFieldsInAsnScope_int(int inputValue, int minValue, int maxValue);

    bool SetBsPayload(Rvs_BS_t bs);//basicVehicleStatus

    bool SetAsPayload(Rvs_AS_t as);//additionalVehicleStatus

    bool SetTsPayload(Rvs_TS_t ts);//temStatus

    bool SetAsCiPayload(Rvs_AS_CI_t as_ci);//additionalVehicleStatus.confAndId

    bool SetAsDsPayload(Rvs_AS_DS_t as_ds);//additionalVehicleStatus.drivingSafetyStatus

    bool SetAsMsPayload(Rvs_AS_MS_t as_ms);//additionalVehicleStatus.maintenanceStatus

    bool SetAsRsPayload(Rvs_AS_RS_t as_rs);//additionalVehicleStatus.runningStatus

    bool SetAsCsPayload(Rvs_AS_CS_t as_cs);//additionalVehicleStatus.climateStatus

    bool SetAsDbPayload(Rvs_AS_DB_t as_db);//additionalVehicleStatus.drivingBehaviourStatus

    bool SetAsEsPayload(Rvs_AS_ES_t as_es);//additionalVehicleStatus.electricVehicleStatus

    bool SetAsPsPayload(Rvs_AS_PS_t as_ps);//additionalVehicleStatus.pollutionStatus

    bool SetAsEwPayload(Rvs_AS_EW_t as_ew);//additionalVehicleStatus.ecuWarningMessages

    bool SetAsCcPayload(Rvs_AS_CC_t as_cc);//additionalVehicleStatus.carConfig

    bool SetAsSignalsKeyPayload(Rvs_AS_SIGNALS_KEY_t as_signals_key);//additionalVehicleStatus.signals.<key>

    bool SetBatteryStatus(const BatteryStatus_Data &appData, BatteryStatus_Model &vdsData);
    bool SetFragStrs(const FragStrs_Data &appData, FragStrs_Model &vdsData);
    bool SetPosition(const Position_Data &appData, Position_Model &vdsData);
    bool SetConfigurationAndIdentity(const ConfigurationAndIdentity_Data &appData, ConfigurationAndIdentity_Model &vdsData);
    bool SetTimeStamp(const TimeStamp_Data &appData, TimeStamp_Model &vdsData);
    bool SetTowStatus(const TowStatus_Data &appData, TowStatus_Model &vdsData);
    bool SetLockStatus(const LockStatus_Data &appData, LockStatus_Model &vdsData);
    bool SetMaintenanceStatus(const MaintenanceStatus_Data &appData, MaintenanceStatus_Model &vdsData);
    bool SetRunningStatus(const RunningStatus_Data &appData, RunningStatus_Model &vdsData);
    bool SetClimateStatus(const ClimateStatus_Data &appData, ClimateStatus_Model &vdsData);
    bool SetPollutionStatus(const PollutionStatus_Data &appData, PollutionStatus_Model &vdsData);
    bool SetElectricStatus(const ElectricStatus_Data &appData, ElectricStatus_Model &vdsData);
    bool SetBasicVehicleStatus(const BasicVehicleStatus_Data &appData, BasicVehicleStatus_Model &vdsData);
    bool SetAdditionalStatus(const AdditionalStatus_Data &appData, AdditionalStatus_Model &vdsData);

    bool SetLockStatusPayload(const LockStatus_Data &payload);
    bool SetMaintenanceStatusPayload(const MaintenanceStatus_Data &payload);
    bool SetRunningStatusPayload(const RunningStatus_Data &payload);
    bool SetClimateStatusPayload(const ClimateStatus_Data &payload);
    bool SetPollutionStatusPayload(const PollutionStatus_Data &payload);
    bool SetElectricStatusPayload(const ElectricStatus_Data &payload);
    bool SetBasicVehicleStatusPayload(const BasicVehicleStatus_Data &payload);
    bool SetAdditionalStatusPayload(const AdditionalStatus_Data &payload);
public:
    RvsSignalAdapt(void * vdsObj);
    bool PackService(RvsRequest_t response);

private:
   VDServiceRequest_Model m_vdsData;
};
}
#endif

