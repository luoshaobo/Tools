/**
* Copyright (C) 2017 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file vc_message_processor.cpp
 * This file handles message processing for VehicleComm
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        25-Jan-2017
 ***************************************************************************/

#include <string>

#include "vuc_comm.hpp"
#include "vehicle_comm.hpp"

#include "vc_utility.hpp"
#include "vc_message_processor.hpp"
#include "local_config_interface.hpp"

namespace vc {

MessageQueue *MessageProcessor::mq_;
DataStorage *MessageProcessor::ds_;

// Private

/**
    @brief Callback function for DataStorage to call when changing a Int param

    This callback is needed in order for message processor to evaluate param changes
    and take action if needed.

    @param[in]  coll_name   Collection name
    @param[in]  par_name    Parameter name
    @param[in]  par_val     Parameter value
*/
void MessageProcessor::ProcessParameterChange(const std::string& coll_name, const ParameterBase& p)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s (coll_name = %s, par_name = %s).", __FUNCTION__, coll_name.c_str(), p.name_.c_str());

    ReturnValue ret;
    MessageBase *message = NULL;

    if  (!coll_name.compare("VehicleState") && !p.name_.compare("vehicleModeMngtGlobalSafeUseMode")) {
        ret = Task_CheckEndOfTrip(p);
        if (ret)
            LOG(LOG_WARN, "MessageProcessor: %s: Failed to check for end-of-trip (err = %s)!",
                    __FUNCTION__, ReturnValueStr[ret]);
        ResCarUsageMode res;
        res.usagemode = (CarUsageModeState)((Parameter<int>*)&p)->value_;
        message = InitMessage((MessageID)EVENT_CARUSAGEMODE, MESSAGE_EVENT, -1, -1, ENDPOINT_MP,
                        ENDPOINT_TS, CREATE_DATA(res));
    } else if (!coll_name.compare("VehicleState") && !p.name_.compare("vehicleModeMngtGlobalSafe")) {
        ResCarMode res;
        res.carmode = (CarModeState)((Parameter<int>*)&p)->value_;
        message = InitMessage((MessageID)EVENT_CARMODE, MESSAGE_EVENT, -1, -1, ENDPOINT_MP,
                        ENDPOINT_TS, CREATE_DATA(res));
    } else if (!coll_name.compare("ElectEnergyLvl") && !p.name_.compare("EgyLvlElecMain")) {
        ResElectEngLvl res;
        res.level = (ElectEngLvl)((Parameter<int>*)&p)->value_;
        message = InitMessage((MessageID)EVENT_ELECTENGLVL, MESSAGE_EVENT, -1, -1,
                        ENDPOINT_MP, ENDPOINT_TS, CREATE_DATA(res));
    } else if (!coll_name.compare("VuCPowerMode") && !p.name_.compare("VuCPowerMode")) {
        ResGetVucPowerMode res;
        res.power_mode = (VucPowerMode)((Parameter<int>*)&p)->value_;
        message = InitMessage((MessageID)EVENT_VUCPOWERMODE, MESSAGE_EVENT, -1, -1,
                        ENDPOINT_MP, ENDPOINT_TS, CREATE_DATA(res));
    } else if (!coll_name.compare("VuCPowerMode") && !p.name_.compare("VuCWakeupReason")) {
        ResGetVucWakeupReason res;
        res.wakeup_reason = (VucWakeupReason)((Parameter<int>*)&p)->value_;
        message = InitMessage((MessageID)EVENT_VUCWAKEUPREASON, MESSAGE_EVENT, -1, -1,
                        ENDPOINT_MP, ENDPOINT_TS, CREATE_DATA(res));
    } else if (!coll_name.compare("VIN") && !p.name_.compare("VinNumber")) {
        ResVINNumber res;
        STRNCPY(res.vin_number, ((Parameter<std::string>*)&p)->value_.c_str(), sizeof(res.vin_number));
        message = InitMessage((MessageID)EVENT_VINNUMBER, MESSAGE_EVENT, -1, -1, ENDPOINT_MP,
                        ENDPOINT_TS, CREATE_DATA(res));
    }

    if (message != NULL)
        (void)mq_->AddMessage(message);
}

/**
    @brief Collects the values o VuC sw build

    Collects the values of VuC software build and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/

ReturnValue MessageProcessor::HandleVucSwBuild(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    ResGetVucSwBuild res;
    Parameter<int>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildDay");
    res.day = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildMonth");
    res.month = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildYear");
    res.year = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildHour");
    res.hour = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildMin");
    res.min = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildSec");
    res.sec = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildSwVersion");
    res.sw_version = par->value_;

    COLL_DSGETINT(par, "VucSwBuild", "VucSwBuildCheckSum");
    res.checksum = par->value_;

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values of BLE sw build

    Collects the values of BLE software build and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleBleSwBuild(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    ResGetBleSwBuild res;
    Parameter<int>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildDay");
    res.day = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildMonth");
    res.month = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildYear");
    res.year = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildHour");
    res.hour = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildMin");
    res.min = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildSec");
    res.sec = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildSwVersion");
    res.sw_version = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleSwBuildCheckSum");
    res.checksum = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleStackVersion_0");
    res.stack_version[0] = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleStackVersion_1");
    res.stack_version[1] = par->value_;

    COLL_DSGETINT(par, "BleSwBuild", "BleStackVersion_2");
    res.stack_version[2] = par->value_;

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));

}

/**
    @brief Collects the values of VuC sw version & partnumber

    Collects the values of VuC software version and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleVucSwVersion(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    ResVucSwVersion res;
    Parameter<std::string>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETSTR(par, "VucSwVersion", "VucPartNumber");
    memcpy(res.vuc_part_number, par->value_.c_str(), LENGTH(res.vuc_part_number));

    COLL_DSGETSTR(par, "VucSwVersion", "VucVersion");
    memcpy(res.vuc_version, par->value_.c_str(), LENGTH(res.vuc_version));

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values of BLE sw version & partnumber

    Collects the values of BLE software version and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleBleSwVersion(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    ResBleSwVersion res;
    Parameter<std::string>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETSTR(par, "BleSwVersion", "BlePartNumber");
    memcpy (res.ble_part_number, par->value_.c_str(), LENGTH(res.ble_part_number));

    COLL_DSGETSTR(par, "BleSwVersion", "BleVersion");
    memcpy (res.ble_version, par->value_.c_str(), LENGTH(res.ble_version));

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values of backup battery statae

    Collects the values of Backup battery state and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleLastBubState(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResLastBubState res;
    Parameter<int>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETINT(par, "BackupBatteryState", "BatteryUsed");
    res.battery_used = static_cast<BatteryUsed>(par->value_);

    COLL_DSGETINT(par, "BackupBatteryState", "State");
    res.battery_state = static_cast<BatteryState>(par->value_);

    COLL_DSGETINT(par, "BackupBatteryState", "ChargeLvl");
    res.charge_level = par->value_;

    COLL_DSGETINT(par, "BackupBatteryState", "BackupBatTemp");
    // Invalid value = 0xFF (implausible value or not available) Sensor error = 0xFE (NTC disconnected, short-circuit etc...)
    // Since temperature is [degC] = -45..+125 but uint_8 is used +45 is added, subract it to get right temperature
    res.backup_battery_temp = par->value_ < 0xfe ? par->value_-45 : par->value_;

    COLL_DSGETINT(par, "BackupBatteryState", "AmbientTemp");
    // Invalid value = 0xFF (implausible value or not available) Sensor error = 0xFE (NTC disconnected, short-circuit etc...)
    // Since temperature is [degC] = -45..+125 but uint_8 is used +45 is added, subract it to get right temperature
    res.ambient_temp = par->value_ < 0xfe ? par->value_-45 : par->value_;

    COLL_DSGETINT(par, "BackupBatteryState", "BatteryVoltage");
    // Invalid value = 0xFFFF (implausible value or not available). According to document [V] = raw*0.01
    res.battery_voltage = par->value_ < 0xffff ? static_cast<float>(par->value_*0.01) : par->value_;

    COLL_DSGETINT(par, "BackupBatteryState", "KL30Voltage");
    // Invalid value = 0xFFFF (implausible value or not available). According to document [V] = raw*0.01
    res.kl30_voltage = par->value_ < 0xffff ? static_cast<float>(par->value_*0.01) : par->value_;

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects 

    
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleCarCfgParaFaults(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    ResCarCfgParameterFault res;
    Parameter<int>* par;

    COLL_DEPREADWRITE_START();
    COLL_DSGETINT(par, "CarCfgParameterFault", "numInvalidCfg");
    res.numInvalidCfg = static_cast<uint8_t>(par->value_);
    COLL_DSGETINT(par, "CarCfgParameterFault", "parameterFault[0].numData");

    res.parameterFault[0].numData = static_cast<uint16_t>(par->value_);
    
    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value of car usage mode

    Collects the values of car usage mode and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleCarUsageMode(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResCarUsageMode res;
    Parameter<int>* par;
    DSGETINT(par, "VehicleState", "vehicleModeMngtGlobalSafeUseMode");
    res.usagemode = static_cast<CarUsageModeState>(par->value_);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value of car mode

    Collects the values of car mode and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleCarMode(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResCarMode res;
    Parameter<int>* par;
    DSGETINT(par, "VehicleState", "vehicleModeMngtGlobalSafe");
    res.carmode = static_cast<CarModeState>(par->value_);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value of electric energy level

    Collects the values of electric energy level and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleElectEngLvl(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResElectEngLvl res;
    Parameter<int>* par;
    DSGETINT(par, "ElectEnergyLvl", "EgyLvlElecMain");
    res.level = static_cast<ElectEngLvl>(par->value_);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value of vuc power mode

    Collects the values of vuc power mode and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetVucPowerMode(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetVucPowerMode res;
    Parameter<int>* par;
    DSGETINT(par, "VuCPowerMode", "VuCPowerMode");
    res.power_mode = static_cast<VucPowerMode>(par->value_);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value of vuc wakeup reason

    Collects the values of vuc wakeup reason and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetVucWakeupReason(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetVucWakeupReason res;
    Parameter<int>* par;
    DSGETINT(par, "VuCPowerMode", "VuCWakeupReason");
    res.wakeup_reason = static_cast<VucWakeupReason>(par->value_);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value of vin number

    Collects the values of vin number and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleVINNumber(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResVINNumber res;
    Parameter<std::string>* par;
    DSGETSTR(par, "VIN", "VinNumber");
    memcpy(res.vin_number, par->value_.c_str(), LENGTH(res.vin_number));
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for ResWifiData

    Collects the values for ResWifiData struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetWifiData(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetWifiData res;
    Parameter<int>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETINT(par, "CarConfig", "WiFiFunctionalityControl");
    res.wifi_functionality_control = static_cast<WiFiFunctionalityControl>(par->value_ & 0xFF);
    res.wifi_functionality_control_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    COLL_DSGETINT(par, "CarConfig", "WiFiFrequencyBand");
    res.wifi_frequency_band = static_cast<WiFiFrequencyBand>(par->value_ & 0xFF);
    res.wifi_frequency_band_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    //COLL_DSGETINT(par, "CarConfig", "WiFiWorkshopMode");
    //res.wifi_workshop_mode = static_cast<WiFiWorkshopMode>(par->value_ & 0xFF);
    //res.wifi_workshop_mode_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    
    DSGETINT(par, "CarConfig", "Connectivity");
    res.connectivity = static_cast<Connectivity>(par->value_ & 0xFF);
    res.connectivity_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for ResPSIMAvailability

    Collects the values for ResPSIMAvailability struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetPSIMAvailability(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetPSIMAvailability res;
    Parameter<int>* par;

    DSGETINT(par, "CarConfig", "PSIMAvailability");
    res.psim_availability = static_cast<PSIMAvailability>(par->value_ & 0xFF);
    res.psim_availability_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for ResGetFuelType

    Collects the values for ResGetFuelType struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetFuelType(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetFuelType res;
    Parameter<int>* par;

    DSGETINT(par, "CarConfig", "FuelType");
    res.fuel_type = static_cast<FuelType>(par->value_ & 0xFF);
    res.fuel_type_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for ResGetPropulsionType

    Collects the values for ResGetPropulsionType struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetPropulsionType(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetPropulsionType res;
    Parameter<int>* par;

    DSGETINT(par, "CarConfig", "PropulsionType");
    res.propulsion_type = static_cast<PropulsionType>(par->value_ & 0xFF);
    res.propulsion_type_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for ohc button state

    Collects the values for ResGetOhcBtnState struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetOhcBtnState(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetOhcBtnState res;
    Parameter<int>* par;

    COLL_DEPREADWRITE_START();

    COLL_DSGETINT(par, "OhcBtnState", "EcallBtn");
    res.ecall_btn = (OhcBtnState)par->value_;

    COLL_DSGETINT(par, "OhcBtnState", "IcallBtn");
    res.icall_btn = (OhcBtnState)par->value_;

    COLL_DEPREADWRITE_STOP();

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for number of doors

    Collects the values for ResGetNumberOfDoors struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetNumberOfDoors(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetNumberOfDoors res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "NumberOfDoors");
    res.nbr_of_doors = static_cast<NbrOfDoors>(par->value_ & 0xFF);
    res.nbr_of_doors_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for steering wheel position

    Collects the values for ResGetSteeringWheelPosition struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetSteeringWheelPosition(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetSteeringWheelPosition res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "SteeringWheelPosition");
    res.steering_wheel_position = static_cast<SteeringWheelPosition>(par->value_ & 0xFF);
    res.steering_wheel_position_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for fuel tank volume

    Collects the values for ResGetFuelTankVolume struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetFuelTankVolume(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetFuelTankVolume res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "FuelTankVolume");
    res.fuel_tank_volume = static_cast<FuelTankVolume>(par->value_ & 0xFF);
    res.fuel_tank_volume_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for assistance services

    Collects the values for ResGetAssistanceServices struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetAssistanceServices(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetAssistanceServices res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "AssistanceServices");
    res.assistance_services = static_cast<AssistanceServices>(par->value_ & 0xFF);
    res.assistance_services_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for Autonomous Drive

    Collects the values for ResGetAutonomousDrive struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetAutonomousDrive(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetAutonomousDrive res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "AutonomousDrive");
    res.autonomous_drive = static_cast<AutonomousDrive>(par->value_ & 0xFF);
    res.autonomous_drive_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for Combined Instrument

    Collects the values for ResGetCombinedInstrument struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetCombinedInstrument(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetCombinedInstrument res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "CombinedInstrument");
    res.combined_instrument = static_cast<CombinedInstrument>(par->value_ & 0xFF);
    res.combined_instrument_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for telematic module

    Collects the values for ResGetTelematicModule struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetTelematicModule(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetTelematicModule res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "TelematicModule");
    res.telematic_module = static_cast<TelematicModule>(par->value_ & 0xFF);
    res.telematic_module_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for connectivity

    Collects the values for ResGetConnectivity struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetConnectivity(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetConnectivity res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "Connectivity");
    res.connectivity = static_cast<Connectivity>(par->value_ & 0xFF);
    res.connectivity_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for connected service booking

    Collects the values for ResGetConnectedServiceBooking struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetConnectedServiceBooking(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetConnectedServiceBooking res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "ConnectedServiceBooking");
    res.connected_service_booking = static_cast<ConnectedServiceBooking>(par->value_ & 0xFF);
    res.connected_service_booking_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for tcam hw version

    Collects the values for ResTcamHwVer struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetTcamHwVersion(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetTcamHwVer res;
    Parameter<int>* parint;
    Parameter<std::string>* parstr;
    Parameter<std::vector<uint8_t>>* parvect;
    COLL_DEPREADWRITE_START();

    COLL_DSGETINT(parint, "TcamHwVersion", "EolHwVersion");
    res.eol_hw_version = static_cast<TCAMEolHwVersion>(parint->value_);
    COLL_DSGETINT(parint, "TcamHwVersion", "EolData");
    res.eol_data = static_cast<TCAMEolData>(parint->value_);
    COLL_DSGETINT(parint, "TcamHwVersion", "EolHwVariant");
    res.eol_hw_variant = static_cast<TCAMEolHwVariant>(parint->value_);
    COLL_DSGETCHARARR(parvect, "TcamHwVersion", "EolVolvoSerialNumber");
    memcpy(res.eol_volvo_ecu_serial_number, (void*)&parvect->value_[0], parvect->value_.size());
    COLL_DSGETSTR(parstr, "TcamHwVersion", "EolVolvoDelivPartNumber");
    memcpy(res.eol_volvo_delivery_part_number, parstr->value_.c_str(), LENGTH(res.eol_volvo_delivery_part_number));
    COLL_DSGETSTR(parstr, "TcamHwVersion", "EolVolvoCorePartNumber");
    memcpy(res.eol_volvo_core_part_number, parstr->value_.c_str(), LENGTH(res.eol_volvo_core_part_number));
    COLL_DSGETINT(parint, "TcamHwVersion", "EolProductionYear");
    res.eol_volvo_production_year = parint->value_;
    COLL_DSGETINT(parint, "TcamHwVersion", "EolProductionMonth");
    res.eol_volvo_production_month = parint->value_;
    COLL_DSGETINT(parint, "TcamHwVersion", "EolProductionDay");
    res.eol_volvo_production_day = parint->value_;
    COLL_DEPREADWRITE_STOP();
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for rf version

    Collects the values for ResRfVersion struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetRfVersion(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetRfVersion res;
    Parameter<std::vector<uint8_t>>* parvect;
    Parameter<int>* parint;
    COLL_DEPREADWRITE_START();
    COLL_DSGETINT(parint, "RfVersion", "RfSwVersion");
    res.sw_version = static_cast<uint16_t>(parint->value_);
    COLL_DSGETCHARARR(parvect, "RfVersion", "RfHwVersion");
    memcpy(res.hw_version, (void*)&parvect->value_[0], parvect->value_.size());
    COLL_DSGETINT(parint, "RfVersion", "RfSwMarketVar");
    res.sw_variant = static_cast<RFSWVariant>(parint->value_);
    COLL_DSGETINT(parint, "RfVersion", "RfHwMarketVar");
    res.hw_variant = static_cast<RFHWVariant>(parint->value_);
    COLL_DSGETCHARARR(parvect, "RfVersion", "RfHwSerialNumber");
    memcpy(res.hw_serial_number, (void*)&parvect->value_[0], parvect->value_.size());
    COLL_DEPREADWRITE_STOP();
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the values for fbl sw build date

    Collects the values for ResFblSwBuild struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetFblSwBuild(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetFblSwBuild res;
    Parameter<int>* par;
    COLL_DEPREADWRITE_START();
    COLL_DSGETINT(par, "FblVersion", "FblSwBuildDay");
    res.day = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwBuildMonth");
    res.month = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwBuildYear");
    res.year = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwBuildHour");
    res.hour = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwBuildMin");
    res.min = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwBuildSec");
    res.sec = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwChecksum");
    res.checksum = par->value_;
    COLL_DSGETINT(par, "FblVersion", "FblSwVersion");
    res.sw_version = par->value_;
    COLL_DEPREADWRITE_STOP();
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

// Test

ReturnValue MessageProcessor::HandleTestSetFakeValuesDefault(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    // HARDCODED VALUES

    ds_->StartReadWriteDepValues();

    // VIN-number
    //ds_->ChangeStringParameter("VIN", "VinNumber", "FAKEVINNUMBER1234", false);

    // Electrical energy level
    ds_->ChangeIntParameter("ElectEnergyLvl", "EgyLvlElecMain", 0, false);

    // PowerMode
    ds_->ChangeIntParameter("VuCPowerMode", "VuCPowerMode", VUCPWM_ACTIVE, false);
    ds_->ChangeIntParameter("VuCPowerMode", "VuCWakeupReason", VUCWAKEUP_BLE_WAKEUP, false);

    // OhcBtnState
    ds_->ChangeIntParameter("OhcBtnState", "EcallBtn", OHCBTN_INACTIVE, false);
    ds_->ChangeIntParameter("OhcBtnState", "IcallBtn", OHCBTN_ERROR, false);
    ds_->StopReadWriteDepValues();

    return SendResponseMessage(mq_, m, NULL);
}


ReturnValue MessageProcessor::HandleTestSetFakeValueInt(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    Message<ReqTestSetFakeValueInt>* mm = (Message<ReqTestSetFakeValueInt>*)m;
    ReqTestSetFakeValueInt *req = &mm->message;

    if (ds_->ChangeIntParameter(std::string(req->collection), std::string(req->name), req->value, false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set fake value int!", __FUNCTION__);
    }

    return SendResponseMessage(mq_, m, NULL);
}

ReturnValue MessageProcessor::HandleTestSetFakeValueStr(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    Message<ReqTestSetFakeValueStr>* mm = (Message<ReqTestSetFakeValueStr>*)m;
    ReqTestSetFakeValueStr *req = &mm->message;

    LOG(LOG_WARN, "VuCComm: %s: collection: %s, name: %s, value: %s", __FUNCTION__, req->collection, req->name, req->value);
    if (ds_->ChangeStringParameter(std::string(req->collection), std::string(req->name), std::string(req->value), false) != RET_OK) {
        LOG(LOG_WARN, "VuCComm: %s: Failed to set fake value str!", __FUNCTION__);
    }

    return SendResponseMessage(mq_, m, NULL);
}

ReturnValue MessageProcessor::HandleGetCarCfgTheftNotification(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetTheftNotification res;
    Parameter<int>* par;

    DSGETINT(par, "CarConfig", "CarCfgTheftNotificaion");
    res.theft_notification = static_cast<TheftNotifcation>(par->value_ & 0xFF);
    res.theft_notification_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

ReturnValue MessageProcessor::HandleGetCarCfgStolenVehicleTracking(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetStolenVehicleTracking res;
    Parameter<int>* par;

    DSGETINT(par, "CarConfig", "CarCfgStolenVehicleTracking");
    res.stolen_vehicle_tracking = static_cast<StolenVehicleTracking>(par->value_ & 0xFF);
    res.stolen_vehicle_tracking_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

ReturnValue MessageProcessor::HandleGetGNSSReceiver(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetGnss res;
    Parameter<int>* par;

    DSGETINT(par, "CarConfig", "GNSSReceiver");
    res.gnss_receiver = static_cast<GnssReceiver>(par->value_ & 0xFF);
    res.gnss_receiver_validity = static_cast<CarConfValidity>(par->value_ >> 8);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for Approved Cellular Standards

    Collects the values for ResGetApprovedCellularStandards struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetApprovedCellularStandards(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetApprovedCellularStandards res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "ApprovedCellularStandards");
    res.approvedcellularstandards = static_cast<ApprovedCellularStandards>(par->value_ & 0xFF);
    res.approvedcellularstandards_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for Digital Video Recorder

    Collects the values for ResGetDigitalVideoRecorder struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetDigitalVideoRecorder(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetDigitalVideoRecorder res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "DigitalVideoRecorder");
    res.digital_video_recorder = static_cast<DigitalVideoRecorder>(par->value_ & 0xFF);
    res.digital_video_recorder_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

/**
    @brief Collects the value for Driver Alert Control

    Collects the values for ResGetDriverAlertControl struct and send
    it back as one reponse.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::HandleGetDriverAlertControl(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetDriverAlertControl res;
    Parameter<int>* par;
    DSGETINT(par, "CarConfig", "DriverAlertControl");
    res.driver_alert_control = static_cast<DriverAlertControl>(par->value_ & 0xFF);
    res.driver_alert_control_validity = static_cast<CarConfValidity>(par->value_ >> 8);
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

////////////////////////////////////////////////////////////
// @brief : Collects the values for door state
//
// @param[in]  m   	Request message
// @return     A VC return value
// @author     ty, Sep 17, 2018
////////////////////////////////////////////////////////////
ReturnValue MessageProcessor::HandleGetDoorState(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResGetDoorState res;
    Parameter<int>* par;        

    memset(&res, 0, sizeof(ResGetDoorState));
    DSGETINT(par, "DoorState", "driverDoorState");
    res.driverDoorState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "driverSideRearDoorState");
    res.driverSideRearDoorState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "driverDoorLockState");
    res.driverDoorLockState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "driverSideRearDoorLockState");
    res.driverSideRearDoorLockState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "passengerDoorState");
    res.passengerDoorState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "passengerSideRearDoorState");
    res.passengerSideRearDoorState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "passengerDoorLockState");
    res.passengerDoorLockState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "passengerSideRearDoorLockState");
    res.passengerSideRearDoorLockState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "tailgateOrBootlidState");
    res.tailgateOrBootlidState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "tailgateOrBootlidLockState");
    res.tailgateOrBootlidLockState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "hoodState");
    res.hoodState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "centralLockingStateForUserFeedback");
    res.centralLockingStateForUserFeedback = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "centralLockState");
    res.centralLockState = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "centralLockStateTrigSource");
    res.centralLockStateTrigSource = static_cast<uint8_t>(par->value_ & 0xFF);
    DSGETINT(par, "DoorState", "centralLockStateNewEvent");
    res.centralLockStateNewEvent = static_cast<uint8_t>(par->value_ & 0xFF);

    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

////////////////////////////////////////////////////////////
// @brief : Collects the values for car's status 
//
// @param[in]  m   	Request message
// @return     A VC return value
// @author     uia93888, Sep 18, 2018
////////////////////////////////////////////////////////////
ReturnValue MessageProcessor::HandleGetRMTStatus(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    ResRMTStatus res = ds_->GetRMTEngineStatus();
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}

////////////////////////////////////////////////////////////
// @brief : Collects the values for engine states 
//
// @param[in]  m   	Request message
// @return     A VC return value
// @author     uia93888, Sep 18, 2018
////////////////////////////////////////////////////////////

ReturnValue MessageProcessor::HandleGetEngineStates(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);
    Res_RMTEngine res = ds_->GetEngineStates();
    return SendResponseMessage(mq_, m, CREATE_DATA(res));
}



// Message-processor tasks

void MessageProcessor::PrintVersionsToFile(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s:", __FUNCTION__);

    if (fp_ == NULL) {
        LOG(LOG_WARN, "MessageProcessor: %s file pointer is NULL", __FUNCTION__);
        return;
    }

    switch ((ResponseID)m->id_) {
    case RES_GETBLESWBUILD: {
        Message<ResGetBleSwBuild> *mm = (Message<ResGetBleSwBuild>*)m;
        LOG_VC_F(fp_, LOG_INFO, "BLE sw build date: %04X-%02X-%02X  time: %02X:%02X:%02X", mm->message.year, mm->message.month, mm->message.day,
                    mm->message.hour, mm->message.min, mm->message.sec);
        LOG_VC_F(fp_, LOG_INFO, "BLE sw checksum: 0x%04X", mm->message.checksum);
        LOG_VC_F(fp_, LOG_INFO, "BLE sw version: 0x%04X", mm->message.sw_version);
        LOG_VC_F(fp_, LOG_INFO, "BLE sw stack version: 0x%02X%02X%02X", mm->message.stack_version[0], mm->message.stack_version[1], mm->message.stack_version[2]);
        break;
    }
    case RES_GETVUCSWBUILD: {
        Message<ResGetVucSwBuild> *mm = (Message<ResGetVucSwBuild>*)m;
        LOG_VC_F(fp_, LOG_INFO, "VuC sw build date: %04X-%02X-%02X  time: %02X:%02X:%02X", mm->message.year, mm->message.month, mm->message.day,
                    mm->message.hour, mm->message.min, mm->message.sec);
        LOG_VC_F(fp_, LOG_INFO, "VuC sw checksum: 0x%04X", mm->message.checksum);
        LOG_VC_F(fp_, LOG_INFO, "VuC sw version: 0x%04X", mm->message.sw_version);
        break;
    }
    case RES_GETVUCBLESWPARTNUMBER: {
        Message<ResGetVucBleSwPartNumber> *mm = (Message<ResGetVucBleSwPartNumber>*)m;
        LOG_VC_F(fp_, LOG_INFO, "VuC version: %.*s", LENGTH(mm->message.vuc_version), mm->message.vuc_version);
        LOG_VC_F(fp_, LOG_INFO, "VuC part number: %.*s", LENGTH(mm->message.vuc_part_number), mm->message.vuc_part_number);
        LOG_VC_F(fp_, LOG_INFO, "BLE version: %.*s", LENGTH(mm->message.ble_version), mm->message.ble_version);
        LOG_VC_F(fp_, LOG_INFO, "BLE part number: %.*s", LENGTH(mm->message.ble_part_number), mm->message.ble_part_number);
        break;
    }
    case RES_GETTCAMHWVERSION: {
        Message<ResGetTcamHwVer> *mm = (Message<ResGetTcamHwVer>*)m;
        if (mm->message.eol_hw_version == TCAM_EOL_HW_VERSION_2_0)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw version: 2.0");
        else if (mm->message.eol_hw_version == TCAM_EOL_HW_VERSION_2_1)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw version: 2.1");
        else if (mm->message.eol_hw_version == TCAM_EOL_HW_VERSION_3_0)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw version: 3.0");
        else if (mm->message.eol_hw_version == TCAM_EOL_HW_VERSION_4_0)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw version: 4.0");
        else if (mm->message.eol_hw_version == TCAM_EOL_HW_VERSION_SOP)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw version: SOP");
        else
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw version: unknown version!");
        if (mm->message.eol_data == TCAM_EOL_DATA_INITIAL_DEF)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol data: initial definition");
        else if (mm->message.eol_data == TCAM_EOL_DATA_NEW_DEFINITION)
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol data: new definition (vuc011.2/E3HW)");
        else
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol data: undefined");
        switch (mm->message.eol_hw_variant) {
        case TCAM_EOL_HW_VARIANT_V1_EU_A2C134793XX:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: TCAM_EOL_HW_VARIANT_V1_EU_A2C134793XX");
            break;
        case TCAM_EOL_HW_VARIANT_V2_NA_A2C134794XX:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: TCAM_EOL_HW_VARIANT_V2_NA_A2C134794XX");
            break;
        case TCAM_EOL_HW_VARIANT_V3_JP_A2C134795XX:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: TCAM_EOL_HW_VARIANT_V3_JP_A2C134795XX");
            break;
        case TCAM_EOL_HW_VARIANT_V4_ROW_A2C134796XX:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: TCAM_EOL_HW_VARIANT_V4_ROW_A2C134796XX");
            break;
        case TCAM_EOL_HW_VARIANT_V5_RU_A2C134797XX:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: TCAM_EOL_HW_VARIANT_V5_RU_A2C134797XX");
            break;
        case TCAM_EOL_HW_VARIANT_V6_CHN_A2C134798XX:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: TCAM_EOL_HW_VARIANT_V6_CHN_A2C134798XX");
            break;
        default:
            LOG_VC_F(fp_, LOG_INFO, "TCAM eol hw variant: undefined");
        }
        LOG_VC_F(fp_, LOG_INFO, "TCAM eol volvo ecu serial number: 0x%02X%02X%02X%02X", mm->message.eol_volvo_ecu_serial_number[0],
                mm->message.eol_volvo_ecu_serial_number[1], mm->message.eol_volvo_ecu_serial_number[2], mm->message.eol_volvo_ecu_serial_number[3]);
        LOG_VC_F(fp_, LOG_INFO, "TCAM eol volvo delivery part number: %.*s", LENGTH(mm->message.eol_volvo_delivery_part_number),mm->message.eol_volvo_delivery_part_number);
        LOG_VC_F(fp_, LOG_INFO, "TCAM eol volvo core part number: %.*s", LENGTH(mm->message.eol_volvo_core_part_number), mm->message.eol_volvo_core_part_number);
        LOG_VC_F(fp_, LOG_INFO, "TCAM eol production date: %04X-%02X-%02X", mm->message.eol_volvo_production_year,
                mm->message.eol_volvo_production_month, mm->message.eol_volvo_production_day);
        break;
    }
    case RES_GETFBLSWBUILD: {
        Message<ResGetFblSwBuild> *mm = (Message<ResGetFblSwBuild>*)m;
        LOG_VC_F(fp_, LOG_INFO, "FBL sw build date: %04X-%02X-%02X  time: %02X:%02X:%02X", mm->message.year, mm->message.month, mm->message.day,
                    mm->message.hour, mm->message.min, mm->message.sec);
        LOG_VC_F(fp_, LOG_INFO, "FBL sw checksum: 0x%04X", mm->message.checksum);
        LOG_VC_F(fp_, LOG_INFO, "FBL sw version: 0x%04X", mm->message.sw_version);
        break;
    }
    case RES_GETRFVERSION: {
        Message<ResGetRfVersion> *mm = (Message<ResGetRfVersion>*)m;
        LOG_VC_F(fp_, LOG_INFO, "RF sw version: %04X", mm->message.sw_version);
        LOG_VC_F(fp_, LOG_INFO, "RF hw version: %02X-%02X-%02X-%02X", mm->message.hw_version[0],
                mm->message.hw_version[1], mm->message.hw_version[2], mm->message.hw_version[3]);

        if (mm->message.sw_variant == RF_SW_VARIANT_EU_US_CH_434MHZ)
            LOG_VC_F(fp_, LOG_INFO, "RF sw variant: RF_SW_VARIANT_EU_US_CH_434MHZ");
        else if (mm->message.sw_variant == RF_SW_VARIANT_JAPAN_315MHZ)
            LOG_VC_F(fp_, LOG_INFO, "RF sw variant: RF_SW_VARIANT_JAPAN_315MHZ");
        else if (mm->message.sw_variant == RF_SW_VARIANT_KOREA_434MHZ)
            LOG_VC_F(fp_, LOG_INFO, "RF sw variant: RF_SW_VARIANT_KOREA_434MHZ");
        if (mm->message.hw_variant == RF_HW_VARIANT_NVM_OF_RF_ERASED)
            LOG_VC_F(fp_, LOG_INFO, "RF hw variant: RF_HW_VARIANT_NVM_OF_RF_ERASED");
        else if (mm->message.hw_variant == RF_HW_VARIANT_V1_434MHZ_RESTOFWORLD_N_KOREA)
            LOG_VC_F(fp_, LOG_INFO, "RF hw variant: RF_HW_VARIANT_V1_434MHZ_RESTOFWORLD_N_KOREA");
        else if (mm->message.hw_variant == RF_HW_VARIANT_V7_315MHZ_JAPAN)
            LOG_VC_F(fp_, LOG_INFO, "RF hw variant: RF_HW_VARIANT_V7_315MHZ_JAPAN");

        LOG_VC_F(fp_, LOG_INFO, "RF hw serial: %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
                mm->message.hw_serial_number[0], mm->message.hw_serial_number[1], mm->message.hw_serial_number[2],mm->message.hw_serial_number[3],
                mm->message.hw_serial_number[4], mm->message.hw_serial_number[5], mm->message.hw_serial_number[6],mm->message.hw_serial_number[7]);
        break;
    }
    case RES_GETMSGCATVERSION: {
        Message<ResGetMsgCatVer> *mm = (Message<ResGetMsgCatVer>*)m;
        if (std::memcmp(mm->message.version, MESSAGE_CATALOGUE_VERSION, sizeof(mm->message.version)) == 0) {
            LOG_VC_F(fp_, LOG_INFO, "Message catalogue matches (nad: %02x.%02x.%02x == vuc: %02x.%02x.%02x)",
                MESSAGE_CATALOGUE_VERSION[0], MESSAGE_CATALOGUE_VERSION[1], MESSAGE_CATALOGUE_VERSION[2],
                mm->message.version[0], mm->message.version[1],mm->message.version[2]);
        }
        else {
            LOG_VC_F(fp_, LOG_INFO, "Message catalogue does not match (nad: %02x.%02x.%02x != vuc: %02x.%02x.%02x)",
                MESSAGE_CATALOGUE_VERSION[0], MESSAGE_CATALOGUE_VERSION[1], MESSAGE_CATALOGUE_VERSION[2],
                mm->message.version[0], mm->message.version[1],mm->message.version[2]);
        }
    }
    default: {
    }
    }
}

/**
    @brief Send request messages needed on Init.

    Send all requests that VehicleComm needs to have on init.
*/
void MessageProcessor::Task_InitRequests()
{
    fp_ = fopen("/tmp/vc_versions","w");
    if (fp_ == NULL) {
        LOG(LOG_WARN, "MessageProcessor: %s: failed to open /tmp/vc_versions for writing.", __FUNCTION__);
        return;
    }

    bool all_response_received_or_no_more_tries = false;

    // Send all init request one at the time
    while (!all_response_received_or_no_more_tries) {
        bool list_is_complete = true;
        for (std::list<MpInitRequestIds>::iterator it = req_list_.begin(); it != req_list_.end(); ++it) {
            if (!it->completed && it->tries > 0) {
                LOG(LOG_DEBUG, "MessageProcessor: %s: Sending: %s.", __FUNCTION__, VCRequestStr[it->request_id]);
                MessageBase *m = InitMessage((MessageID)it->request_id, MESSAGE_REQUEST, 0,
                                            it->session, ENDPOINT_MP, it->endpoint,
                                            (const unsigned char*)it->data);
                if (m == NULL) {
                    LOG(LOG_ERR, "MessageProcessor: %s: Message initiated to NULL aborting.", __FUNCTION__);
                    return;
                }
                cur_session_init_req_ = m->session_id_;
                mq_->AddMessage(m);
                std::unique_lock<std::mutex> lk(mtx_init_requests_);
                cv_init_requests_.wait(lk);
                list_is_complete = false;
                it->tries--;
                if (exit_)
                    return;
            }
        }
        all_response_received_or_no_more_tries = list_is_complete;
    }

    // Clear up request queue
    for (std::list<MpInitRequestIds>::iterator it = req_list_.begin(); it != req_list_.end();) {
        if (it->data != NULL) {
            delete (it->data);
        }
        it = req_list_.erase(it);
    }

    fclose(fp_);
    fp_ = NULL;
    LOG(LOG_DEBUG, "MessageProcessor: %s: All done! Terminating thread.", __FUNCTION__);
}

ReturnValue MessageProcessor::HandleInitResponse(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s: %s.", __FUNCTION__, ResponseStr[m->id_]);
    for (std::list<MpInitRequestIds>::iterator it = req_list_.begin(); it != req_list_.end(); ++it) {
        if (it->request_id == RequestLookup((ResponseID)m->id_) && !it->completed && it->session == m->session_id_) {
            it->completed = true;
            cv_init_requests_.notify_one();
            return RET_OK;
        }
    }
    LOG(LOG_WARN, "MessageProcessor: %s: Couldnt find %s in request list.", __FUNCTION__, VCRequestStr[RequestLookup((ResponseID)m->id_)]);
    return RET_ERR_INTERNAL;
}
/**
    @brief Handle the conditions for End-of-trip event

    Check the values related to end-of-trip and send message if
    fulfilled.
    @param[in]  m     Request message
    @return     A VC return value
*/
ReturnValue MessageProcessor::Task_CheckEndOfTrip(const ParameterBase& p)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    if (p.name_.compare("vehicleModeMngtGlobalSafeUseMode"))
        return RET_ERR_INVALID_ARG;

    bool send_eot = false;
    CarUsageModeState usage_mode_new_endoftrip = (CarUsageModeState)((Parameter<int>*)&p)->value_;
    if ((usage_mode_endoftrip != usage_mode_new_endoftrip) && (usage_mode_endoftrip == CAR_DRIVING)) 
    {
        if (usage_mode_new_endoftrip == CAR_ABANDONED) {
            send_eot = true;
        } else {
            Parameter<int>* par;
            DSGETINT(par, "DoorState", "driverDoorState");
            if ((par->value_ & 0xFF) == DOORSTATE_OPEN) {
                send_eot = true;
            }
        }    
    }
    usage_mode_endoftrip = usage_mode_new_endoftrip;

    if (!send_eot)
        return RET_OK;

    LOG(LOG_DEBUG, "MessageProcessor: %s: Conditions for end-of-trip fulfilled.", __FUNCTION__);

    EventEndOfTrip event;
    MessageBase *m = InitMessage((MessageID)EVENT_ENDOFTRIP, MESSAGE_EVENT, -1, -1, ENDPOINT_MP, CREATE_DATA(event));
    if (m == NULL) {
        LOG(LOG_WARN, "DataStorage: %s InitMessage failed!", __FUNCTION__);
        return RET_ERR_INTERNAL;
    }
    m->dst_ = ENDPOINT_TS;
    (void)mq_->AddMessage(m);

    return RET_OK;
}



// Public

/**
    @brief Constructor for MessageProcessor.

    Constructor for MessageProcessor.
*/
MessageProcessor::MessageProcessor(MessageQueue *mq, DataStorage *ds)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    mq_ = mq;
    ds_ = ds;
    cur_session_init_req_ = -1;
    fp_ = NULL;
}

/**
    @brief Destructor for MessageProcessor.

    Destructor for MessageProcessor.
*/
MessageProcessor::~MessageProcessor()
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    Stop();
}

/**
    @brief Init the MessageProccessor instance.

    This method takes care of the initialization work needed
    for MessageProccessor.
    @return     A VC return value
*/
ReturnValue MessageProcessor::Init()
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    ReqGetMsgCatVer *req_msgcat_ = new ReqGetMsgCatVer { MESSAGE_CATALOGUE_VERSION[0], MESSAGE_CATALOGUE_VERSION[1], MESSAGE_CATALOGUE_VERSION[2] };
    req_list_.push_back({ .request_id = REQ_GETMSGCATVERSION, .data = (char*)req_msgcat_, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETTCAMHWVERSION, .data ={}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETVUCSWBUILD, .data ={}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETVUCBLESWPARTNUMBER, .data ={}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETBLESWBUILD, .data ={}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETFBLSWBUILD, .data ={}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETRFVERSION, .data ={}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_VINNUMBER, .data = {}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 1, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETVEHICLESTATE, .data = {}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 1, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETOHCBTNSTATE, .data = {}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 1, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_LASTBUBSTATE, .data = {}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 1, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_ELECTENGLVL, .data = {}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 1, .session = VehicleComm::GetSessionID()});
    req_list_.push_back({ .request_id = REQ_GETDIAGERRREPORT, .data = {}, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 1, .session = VehicleComm::GetSessionID()});

    for (uint8_t i = 0; i < LENGTH(car_config_params); i++) {
        ReqGetCarConfig *req = new ReqGetCarConfig{ .param_id = (uint16_t)car_config_params[i].car_config_id };
        req_list_.push_back({ .request_id = REQ_GETCARCONFIG, .data = (char*)req, .endpoint = ENDPOINT_VUC, .completed = false, .tries = 5, .session = VehicleComm::GetSessionID()});
    }
    return RET_OK;
}

/**
    @brief Start MessageProcessor.

    This method starts the execution of message-processor.
    @return     A VC return value
*/
ReturnValue MessageProcessor::Start()
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    //thr_carconfig_ = std::thread(&MessageProcessor::Task_CarConfig, this);
    thr_init_requests_ = std::thread(&MessageProcessor::Task_InitRequests, this);

    return RET_OK;
}

/**
    @brief Stop MessageProcessor.

    This method stops the execution of the MessageProcessor.
    @return     A VC return value
*/
ReturnValue MessageProcessor::Stop()
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    exit_ = true;
    cv_init_requests_.notify_all();

    if(thr_init_requests_.joinable())
        thr_init_requests_.join();

    return RET_OK;
}

/**
    @brief Send message to MessageProcessor.

    This method is used by the user to send a request to MessageProcessor.

    @return     A VC return-value
*/
ReturnValue MessageProcessor::SendMessage(MessageBase *m)
{
    LOG(LOG_DEBUG, "MessageProcessor: %s.", __FUNCTION__);

    if (m == NULL) {
        LOG(LOG_WARN, "MessageProcessor: %s: Message is NULL!", __FUNCTION__);
        return RET_ERR_INVALID_ARG;
    }

    LOG(LOG_DEBUG, "MessageProcessor: %s: Incoming message %s", __FUNCTION__, MessageStr(m));


    ReturnValue ret;

    switch (m->type_) {
    case MESSAGE_REQUEST: {
        switch ((RequestID)m->id_) {
        case REQ_GETVUCSWBUILD: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETVUCSWBUILD.", __FUNCTION__);
            ret = HandleVucSwBuild(m);
            break;
        }
        case REQ_GETBLESWBUILD: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETBLESWBUILD.", __FUNCTION__);
            ret = HandleBleSwBuild(m);
            break;
        }
        case REQ_VUCSWVERSION: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_VUCSWVERSION.", __FUNCTION__);
            ret = HandleVucSwVersion(m);
            break;
        }
        case REQ_BLESWVERSION: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_BLESWVERSION.", __FUNCTION__);
            ret = HandleBleSwVersion(m);
            break;
        }
        case REQ_LASTBUBSTATE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_LASTBUBSTATE.", __FUNCTION__);
            ret = HandleLastBubState(m);
            break;
        }
        case REQ_CARUSAGEMODE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_USAGEMODE.", __FUNCTION__);
            ret = HandleCarUsageMode(m);
            break;
        }
        case REQ_CARMODE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_CARMODE.", __FUNCTION__);
            ret = HandleCarMode(m);
            break;
        }
        case REQ_ELECTENGLVL: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_ELECTENGLVL.", __FUNCTION__);
            ret = HandleElectEngLvl(m);
            break;
        }
        case REQ_GETVUCPOWERMODE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETVUCPOWERMODE.", __FUNCTION__);
            ret = HandleGetVucPowerMode(m);
            break;
        }
        case REQ_GETVUCWAKEUPREASON: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETVUCWAKEUPREASON.", __FUNCTION__);
            ret = HandleGetVucWakeupReason(m);
            break;
        }
        case REQ_VINNUMBER: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_VINNUMBER.", __FUNCTION__);
            ret = HandleVINNumber(m);
            break;
        }
        case REQ_GETWIFIDATA: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETWIFIDATA.", __FUNCTION__);
            ret = HandleGetWifiData(m);
            break;
        }
        case REQ_GETPSIMAVAILABILITY: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETWIFIDATA.", __FUNCTION__);
            ret = HandleGetPSIMAvailability(m);
            break;
        }
        case REQ_GETFUELTYPE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETFUELTYPE.", __FUNCTION__);
            ret = HandleGetFuelType(m);
            break;
        }
        case REQ_GETPROPULSIONTYPE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETPROPULSIONTYPE.", __FUNCTION__);
            ret = HandleGetPropulsionType(m);
            break;
        }
        case REQ_GETOHCBTNSTATE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETOHCBTNSTATE.", __FUNCTION__);
            ret = HandleGetOhcBtnState(m);
            break;
        }
        case REQ_GETNUMBEROFDOORS: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETNUMBEROFDOORS.", __FUNCTION__);
            ret = HandleGetNumberOfDoors(m);
            break;
        }
        case REQ_GETSTEERINGWHEELPOSITION: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETSTEERINGWHEELPOSITION.", __FUNCTION__);
            ret = HandleGetSteeringWheelPosition(m);
            break;
        }
        case REQ_GETFUELTANKVOLUME: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETFUELTANKVOLUME.", __FUNCTION__);
            ret = HandleGetFuelTankVolume(m);
            break;
        }
        case REQ_GETASSISTANCESERVICES: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETASSISTANCESERVICES.", __FUNCTION__);
            ret = HandleGetAssistanceServices(m);
            break;
        }
        case REQ_GETAUTONOMOUSDRIVE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETAUTONOMOUSDRIVE.", __FUNCTION__);
            ret = HandleGetAutonomousDrive(m);
            break;
        }
        case REQ_GETCOMBINEDINSTRUMENT: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETCOMBINEDINSTRUMENT.", __FUNCTION__);
            ret = HandleGetCombinedInstrument(m);
            break;
        }
        case REQ_GETTELEMATICMODULE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETTELEMATICMODULE.", __FUNCTION__);
            ret = HandleGetTelematicModule(m);
            break;
        }
        case REQ_GETCONNECTIVITY: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETCONNECTIVITY.", __FUNCTION__);
            ret = HandleGetConnectivity(m);
            break;
        }
        case REQ_GETCONNECTEDSERVICEBOOKING: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETCONNECTEDSERVICEBOOKING.", __FUNCTION__);
            ret = HandleGetConnectedServiceBooking(m);
            break;
        }
        case REQ_GETTCAMHWVERSION: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETTCAMHWVERSION.", __FUNCTION__);
            ret = HandleGetTcamHwVersion(m);
            break;
        }
        case REQ_GETRFVERSION: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETRFVERSION.", __FUNCTION__);
            ret = HandleGetRfVersion(m);
            break;
        }
        case REQ_GETFBLSWBUILD: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETFBLSWBUILD.", __FUNCTION__);
            ret = HandleGetFblSwBuild(m);
            break;
        }
        case REQ_GETCARCFGTHEFTNOTIFICATION: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETCARCFGTHEFTNOTIFICATION.", __FUNCTION__);
            ret = HandleGetCarCfgTheftNotification(m);
            break;
        }
        case REQ_GETCARCFGSTOLENVEHICLETRACKING: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETCARCFGSTOLENVEHICLETRACKING.", __FUNCTION__);
            ret = HandleGetCarCfgStolenVehicleTracking(m);
            break;
        }
        case REQ_GETGNSSRECEIVER: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETGNSSRECEIVER.", __FUNCTION__);
            ret = HandleGetGNSSReceiver(m);
            break;
        }
        case REQ_GETAPPROVEDCELLULARSTANDARDS: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETAPPROVEDCELLULARSTANDARDS.", __FUNCTION__);
            ret = HandleGetApprovedCellularStandards(m);
            break;
        }
        case REQ_GETDIGITALVIDEORECORDER: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETDIGITALVIDEORECORDER.", __FUNCTION__);
            ret = HandleGetDigitalVideoRecorder(m);
            break;
        }
        case REQ_GETDRIVERALERTCONTROL: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETDRIVERALERTCONTROL.", __FUNCTION__);
            ret = HandleGetDriverAlertControl(m);
            break;
        }
        case REQ_GETDOORSTATE: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETDOORSTATE.", __FUNCTION__);
            ret = HandleGetDoorState(m);
            break;
        }
        //uia9388
        case REQ_GETRMTSTATUS:{
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETRMTSTATUS.", __FUNCTION__);
            ret = HandleGetRMTStatus(m);
            break;
        }
        case REQ_GETENGINESTATES:{
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_GETRMTSTATUS.", __FUNCTION__);
            ret = HandleGetEngineStates(m);
            break;
        }

        // Test

        case REQ_TEST_SETFAKEVALUESDEFAULT: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_TEST_SETFAKEVALUESDEFAULT.", __FUNCTION__);
            ret = HandleTestSetFakeValuesDefault(m);
            break;
        }
        case REQ_TEST_SETFAKEVALUEINT: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_TEST_SETFAKEVALUEINT.", __FUNCTION__);
            ret = HandleTestSetFakeValueInt(m);
            break;
        }
        case REQ_TEST_SETFAKEVALUESTR: {
            LOG(LOG_DEBUG, "MessageProcessor: %s REQ_TEST_SETFAKEVALUESTR.", __FUNCTION__);
            ret = HandleTestSetFakeValueStr(m);
            break;
        }

        default:
            LOG(LOG_WARN, "MessageProcessor: %s Unknown request-ID (%d)!", __FUNCTION__, m->id_);
            ret = RET_ERR_INTERNAL;
        }

        if (ret != RET_OK)
            SendErrorMessage(mq_, m, ret);

        break;
    }
    case MESSAGE_RESPONSE: {
        switch ((ResponseID)m->id_) {
        case RES_GETBLESWBUILD:
        case RES_GETVUCSWBUILD:
        case RES_GETVUCBLESWPARTNUMBER:
        case RES_GETTCAMHWVERSION:
        case RES_GETFBLSWBUILD:
        case RES_GETRFVERSION:
        case RES_GETMSGCATVERSION:
            PrintVersionsToFile(m);
        case RES_GETVEHICLESTATE:
        case RES_GETOHCBTNSTATE:
        case RES_LASTBUBSTATE:
        case RES_ELECTENGLVL:
        case RES_GETDIAGERRREPORT:
        case RES_GETCARCONFIG:
        case RES_VINNUMBER: {
            ret = HandleInitResponse(m);
            break;
        }
        default:
            LOG(LOG_WARN, "MessageProcessor: %s: Unknown response-ID (%d)!", __FUNCTION__, m->id_);
            ret = RET_ERR_INTERNAL;
            break;
        }

        break;
    }
    case MESSAGE_EVENT: {
        switch ((EventID)m->id_) {
        case EVENT_DSPARAMETER_VECT: {
            LOG(LOG_DEBUG, "MessageProcessor: %s: EVENT_DSPARAMETER_VECT.", __FUNCTION__);
            Message<EventDSParameterVect> *mm = (Message<EventDSParameterVect>*)m;
            EventDSParameterVect *par = &mm->message;
            Parameter<std::vector<unsigned char>> p(par->name, std::vector<unsigned char>(par->value, par->value+sizeof(par->value)));
            std::string coll(par->collection);
            ProcessParameterChange(coll, p);
            break;
        }
        case EVENT_DSPARAMETER_STR: {
            LOG(LOG_DEBUG, "MessageProcessor: %s: EVENT_DSPARAMETER_STR.", __FUNCTION__);
            Message<EventDSParameterStr> *mm = (Message<EventDSParameterStr>*)m;
            EventDSParameterStr *par = &mm->message;
            Parameter<std::string> p(par->name, par->value);
            std::string coll(par->collection);
            ProcessParameterChange(coll, p);
            break;
        }
        case EVENT_DSPARAMETER_INT: {
            LOG(LOG_DEBUG, "MessageProcessor: %s: EVENT_DSPARAMETER_INT.", __FUNCTION__);
            Message<EventDSParameterInt> *mm = (Message<EventDSParameterInt>*)m;
            EventDSParameterInt *par = &mm->message;
            Parameter<int> p(par->name, par->value);
            std::string coll(par->collection);
            ProcessParameterChange(coll, p);
            break;
        }
        default:
            break;
        }

        ret = RET_OK;

        break;
    }
    case MESSAGE_ERROR: {
        LOG(LOG_WARN, "MessageProcessor: %s: Error-message received (%s)!", __FUNCTION__, ReturnValueStr[m->id_]);
        if (cur_session_init_req_ == m->session_id_) {
            cur_session_init_req_ = -1;
            cv_init_requests_.notify_one();
        }

        ret = RET_OK;
        break;
    }
    default:
        ret = RET_ERR_INVALID_ARG;
        break;
    }

    return ret;
}

}
