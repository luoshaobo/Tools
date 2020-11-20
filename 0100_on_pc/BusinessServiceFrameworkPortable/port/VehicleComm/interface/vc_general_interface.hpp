#ifndef VC_GENERAL_INTERFACE_HPP
#define VC_GENERAL_INTERFACE_HPP

/**
* Copyright (C) 2016 Continental Automotive GmbH
*
* Licensed under the "Supplier unrestricted white box" category, as
* described in the 20160322-1_SW-amendment TCAM.pdf agreement
* between Volvo Cars and Continental Automotive GmbH.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/** @file blem_pairing_service_interface.hpp
 * This file implements a library to talk to VCanager (pairing service) over dbus
 *
 * @project     VCC
 * @subsystem   VehicleComm
 * @version     00.01
 * @author      Ola Lilja & Johan Bohlin
 * @date        22-Dec-2017
 ***************************************************************************/

#include "vc_common.hpp"
#include "vehicle_comm_types.hpp"

namespace vc {

/**
    @brief BLE-interface for external users.

    This interface is used to perform interactions with the VehicleComm, related to the BLE.
*/
class IGeneral {
  public:
    // Requests
    virtual ReturnValue Request_LockDoor(ReqDoorLockUnlock *req, long session_id) = 0;
    virtual ReturnValue Request_HornAndLight(ReqHornNLight *req, long session_id) = 0;
    virtual ReturnValue Request_AntennaSwitch(ReqAntennaSwitch *req, long session_id) = 0;
    virtual ReturnValue Request_LanLinkTrigger(ReqLanLinkTrigger *req, long session_id) = 0;
    virtual ReturnValue Request_AdcValue(ReqAdcValue *req, long session_id) = 0;
    virtual ReturnValue Request_VINNumber(long session_id) = 0;
    virtual ReturnValue Request_CarUsageMode(long session_id) = 0;
    virtual ReturnValue Request_CarMode(long session_id) = 0;
    virtual ReturnValue Request_ElectEnergyLevel(long session_id) = 0;
    virtual ReturnValue Request_GetWifiData(long session_id) = 0;
    virtual ReturnValue Request_GetPSIMAvailability(long session_id) = 0;
    virtual ReturnValue Request_GetFuelType(long session_id) = 0;
    virtual ReturnValue Request_GetPropulsionType(long session_id) = 0;
    virtual ReturnValue Request_IpWakeup(ReqIpWakeup *req, long session_id) = 0;
    virtual ReturnValue Request_SetOhcLed(ReqSetOhcLed *req, long session_id) = 0;
    virtual ReturnValue Request_GetVucPowerMode(long session_id) = 0;
    virtual ReturnValue Request_GetVucWakeupReason(long session_id) = 0;
    virtual ReturnValue Request_GetOhcBtnState(long session_id) = 0;
    virtual ReturnValue Request_GetNumberOfDoors(long session_id) = 0;
    virtual ReturnValue Request_GetSteeringWheelPosistion(long session_id) = 0;
    virtual ReturnValue Request_GetFuelTankVolume(long session_id) = 0;
    virtual ReturnValue Request_GetAssistanceServices(long session_id) = 0;
    virtual ReturnValue Request_GetAutonomousDrive(long session_id) = 0;
    virtual ReturnValue Request_GetCombinedInstrument(long session_id) = 0;
    virtual ReturnValue Request_GetTelematicModule(long session_id) = 0;
    virtual ReturnValue Request_GetConnectivity(long session_id) = 0;
    virtual ReturnValue Request_GetConnectedServiceBooking(long session_id) = 0;
    virtual ReturnValue Request_GetBeltInformation(long session_id) = 0;
    virtual ReturnValue Request_SendPosnFromSatltCon(ReqSendPosnFromSatltCon req, long session_id) = 0;
    virtual ReturnValue Request_GetDiagErrReport(long session_id) = 0;
    virtual ReturnValue Request_PSIMData(ReqPSIMData *req, long session_id) = 0;
    virtual ReturnValue Request_ThermalMitigation(ReqThermalMitigataion *req, long session_id) = 0;
    virtual ReturnValue Request_GetCarCfgTheftNotification(long session_id) = 0;
    virtual ReturnValue Request_GetCarCfgStolenVehicleTracking(long session_id) = 0;
    virtual ReturnValue Request_GetGNSSReceiver(long session_id) = 0;
    virtual ReturnValue Request_GetApprovedCellularStandards(long session_id) = 0;
    virtual ReturnValue Request_GetDigitalVideoRecorder(long session_id) = 0;
    virtual ReturnValue Request_GetDriverAlertControl(long session_id) = 0;
#if 1 // nieyj
    virtual ReturnValue Request_WindowsCtrl(ReqWinCtrl *req, long session_id) = 0;
    virtual ReturnValue Request_SunRoofAndCurtCtrl(ReqRoofCurtCtrl *req, long session_id) = 0;
    virtual ReturnValue Request_WinVentilation(ReqWinVentilation *req, long session_id) = 0;
    virtual ReturnValue Request_PM25Enquire(long session_id) = 0;
    virtual ReturnValue Request_VFCActivate(ReqVFCActivate *req, long session_id) = 0;
#endif
    //uia93888 remote start engine
    virtual ReturnValue Request_SendRMTEngine(Req_RMTEngine* req, long session_id) = 0;
    virtual ReturnValue Request_RmtEngineSecurityRespose(Req_RMTEngineSecurityResponse* req, long session_id) = 0;
    virtual ReturnValue Request_GetRMTStatus(long session_id) = 0;
    virtual ReturnValue Request_GetEngineStates(long session_id) = 0;
    virtual ReturnValue Request_GetDoorState(long session_id) = 0;
    virtual ReturnValue Request_RTCAwakeVuc(Req_RTCAwakeVuc* req, long session_id) = 0;
    virtual ReturnValue Request_DelayEngineRunningTime(Req_DelayEngineRunngTime* req, long session_id) = 0;
    virtual ReturnValue Request_RemoteClimateOperate(Req_OperateRMTClimate* req, long session_id) = 0;
    virtual ReturnValue Request_RMTSeatHeat(vc::Req_OperateRMTSeatHeat* req, long session_id) = 0;
    virtual ReturnValue Request_RMTSeatVentilation(vc::Req_OperateRMTSeatVenti* req, long session_id) = 0;
    virtual ReturnValue Request_RMTSteerWhlHeat(vc::Req_OperateRMTSteerWhlHeat_S* req, long session_id) = 0;
    virtual ReturnValue Request_ParkingClimateOperate(vc::Req_ParkingClimateOper_S* req, long session_id) = 0;
    //end uia93888

    // Test
    virtual ReturnValue Request_VucTraceConfig(ReqVucTraceConfig *req, long session_id) = 0;
    virtual ReturnValue Request_VucTraceControl(ReqVucTraceControl *req, long session_id) = 0;
    virtual ReturnValue Request_GetDSParameter(ReqGetDSParameter&& req, long session_id) = 0;
    virtual ReturnValue Request_VGMTest(ReqVGMTestMsg *req, long session_id) = 0;
    virtual ReturnValue Request_IHUTest(ReqIHUTestMsg *req, long session_id) = 0;
    virtual ReturnValue Event_Test_FromVC(EventTestFromVC *ev, long session_id) = 0;
    virtual ReturnValue Request_Test_ToggleUsageMode(long session_id) = 0;
    virtual ReturnValue Request_Test_ToggleCarMode(long session_id) = 0;
    virtual ReturnValue Request_Test_CrashState(long session_id) = 0;
    virtual ReturnValue Request_Test_OhcState(long session_id) = 0;
    virtual ReturnValue Request_Test_SetFakeValuesDefault(long session_id) = 0;
    virtual ReturnValue Request_Test_SetFakeValueInt(ReqTestSetFakeValueInt *req, long session_id) = 0;
    virtual ReturnValue Request_Test_SetFakeValueStr(ReqTestSetFakeValueStr *req, long session_id) = 0;
    virtual ReturnValue Request_Test_SetLocalConfig(ReqTestSetLocalConfig *req, long session_id) = 0;
};

}; // namespace vc

#endif // VC_GENERAL_INTERFACE_HPP
