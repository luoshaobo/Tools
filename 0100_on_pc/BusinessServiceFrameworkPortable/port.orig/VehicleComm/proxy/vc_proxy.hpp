#ifndef VEHICLE_COMM_CLIENT_HPP
#define VEHICLE_COMM_CLIENT_HPP

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
 *****************ReturnValue**********************************************************/

#include "vc_common.hpp"
#include "vehicle_comm_interface.hpp"

namespace vc {

static const GDBusErrorEntry vehiclecomm_error_entries[] =
{
    {RET_OK,                       "com.contiautomotive.tcam.VehicleComm.Error.None"},
    {RET_ERR_INVALID_ARG,          "com.contiautomotive.tcam.VehicleComm.Error.InvalidArgument"},
    {RET_ERR_INTERNAL,             "com.contiautomotive.tcam.VehicleComm.Error.Internal"},
    {RET_ERR_EXTERNAL,             "com.contiautomotive.tcam.VehicleComm.Error.External"},
    {RET_ERR_OUT_OF_MEMORY,        "com.contiautomotive.tcam.VehicleComm.Error.OutOfMemory"},
    {RET_ERR_TIMEOUT,              "com.contiautomotive.tcam.VehicleComm.Error.Timeout"},
    {RET_ERR_BUSY,                 "com.contiautomotive.tcam.VehicleComm.Error.Busy"},
    {RET_ERR_VALUE_NOT_AVAILABLE,  "com.contiautomotive.tcam.VehicleComm.Error.ValueNotAvailable"},
};

class VehicleCommClient : public IVehicleComm {
  private:
    static guint16 client_id_;                      /**< The client-ID that is provided by VehicleComm */
    bool initialized_;
    Endpoint src_;                                /**< The endpoint-ID that represent this client */

    // Responses
    ResponseCallback response_callback_;          /**< The callback registered for responses to requests */
    VehicleCommIface *ts_comm_request_proxy_;           /**< The GDBus proxy-method used for request/response */
    gulong response_status_;
    static void ResponseCb(VehicleCommIface *ts_comm_request_proxy_, GAsyncResult *res, gpointer user_data);
    
    // Events
    EventCallback event_callback_;                /**< The callback registered for events */
    gulong event_status_general_;
    gulong event_status_ble_;
    gulong event_status_diagnostics_;
    VehicleCommIface *ts_comm_event_proxy_;              /**< The GDBus proxy-method used for events */
    static void EventCb(VehicleCommIface *ifc, guint16 messageid, guint uniqueid, guint16 clientid,
                    GVariant *payload, guint16 payloadsize);

    // System-events
    gulong event_status_system_    ;
    static void SystemEventCb(VehicleCommIface *ifc, guint16 event_id);

    MessageBase *CreateRequest(RequestID request_id, const unsigned char *data, long session_id);
    ReturnValue SendRequest(MessageBase *message);
    
    // Test
    MessageBase *CreateEvent(EventID request_id, const unsigned char *data, long session_id);

  public:
    VehicleCommClient();
    ~VehicleCommClient();

    // IVehicleCommClient
    ReturnValue Init(ResponseCallback response_callback, EventCallback event_callback) override;
    ReturnValue EnableGeneralEvents(void) override;
    ReturnValue DisableGeneralEvents(void) override;
    ReturnValue EnableBLEEvents(void) override;
    ReturnValue DisableBLEEvents(void) override;
    ReturnValue EnableDiagnosticsEvents(void) override;
    ReturnValue DisableDiagnosticsEvents(void) override;
    
    // IGeneral
    ReturnValue Request_LockDoor(ReqDoorLockUnlock *req, long session_id) override;
    ReturnValue Request_HornAndLight(ReqHornNLight *req, long session_id) override;
    ReturnValue Request_AntennaSwitch(ReqAntennaSwitch *req, long session_id) override;
    ReturnValue Request_LanLinkTrigger(ReqLanLinkTrigger *req, long session_id) override;
    ReturnValue Request_AdcValue(ReqAdcValue *req, long session_id) override;
    ReturnValue Request_VINNumber(long session_id) override;
    ReturnValue Request_CarUsageMode(long session_id) override;
    ReturnValue Request_CarMode(long session_id) override;
    ReturnValue Request_ElectEnergyLevel(long session_id) override;
    ReturnValue Request_GetWifiData(long session_id) override;
    ReturnValue Request_GetPSIMAvailability(long session_id) override;
    ReturnValue Request_GetFuelType(long session_id) override;
    ReturnValue Request_GetPropulsionType(long session_id) override;
    ReturnValue Request_IpWakeup(ReqIpWakeup *req, long session_id) override;
    ReturnValue Request_SetOhcLed(ReqSetOhcLed *req, long session_id) override;
    ReturnValue Request_GetVucPowerMode(long session_id) override;
    ReturnValue Request_GetVucWakeupReason(long session_id) override;
    ReturnValue Request_GetOhcBtnState(long session_id) override;
    ReturnValue Request_GetNumberOfDoors(long session_id) override;
    ReturnValue Request_GetSteeringWheelPosistion(long session_id) override;
    ReturnValue Request_GetFuelTankVolume(long session_id) override;
    ReturnValue Request_GetAssistanceServices(long session_id) override;
    ReturnValue Request_GetAutonomousDrive(long session_id) override;
    ReturnValue Request_GetCombinedInstrument(long session_id) override;
    ReturnValue Request_GetTelematicModule(long session_id) override;
    ReturnValue Request_GetConnectivity(long session_id) override;
    ReturnValue Request_GetConnectedServiceBooking(long session_id) override;
    ReturnValue Request_GetBeltInformation(long session_id) override;   
    ReturnValue Request_SendPosnFromSatltCon(ReqSendPosnFromSatltCon req, long session_id) override;
    ReturnValue Request_GetDiagErrReport(long session_id) override;
    ReturnValue Request_PSIMData(ReqPSIMData *req, long session_id) override;
    ReturnValue Request_ThermalMitigation(ReqThermalMitigataion *req, long session_id) override;
    ReturnValue Request_GetCarCfgTheftNotification(long session_id) override;
    ReturnValue Request_GetCarCfgStolenVehicleTracking(long session_id) override;
    ReturnValue Request_GetGNSSReceiver(long session_id) override;
    ReturnValue Request_GetApprovedCellularStandards(long session_id) override;
    ReturnValue Request_GetDigitalVideoRecorder(long session_id) override;
    ReturnValue Request_GetDriverAlertControl(long session_id) override;
    ReturnValue Request_CarconfigParameterFaults(long session_id) override;
    ReturnValue Request_NetworkManagementHistory(long session_id) override;
    ReturnValue Request_ProgramPreCheck(long session_id) override;
#if 1 // nieyj
    ReturnValue Request_WindowsCtrl(ReqWinCtrl *req, long session_id) override;
    ReturnValue Request_SunRoofAndCurtCtrl(ReqRoofCurtCtrl *req, long session_id) override;
    ReturnValue Request_WinVentilation(ReqWinVentilation *req, long session_id) override;
    ReturnValue Request_PM25Enquire(long session_id) override;
    ReturnValue Request_VFCActivate(ReqVFCActivate *req, long session_id) override;
#endif
    //uia93888 remote start engine
    ReturnValue Request_SendRMTEngine(Req_RMTEngine* req, long session_id) override;
    ReturnValue Request_RmtEngineSecurityRespose(Req_RMTEngineSecurityResponse* req, long session_id) override;
    ReturnValue Request_GetRMTStatus(long session_id) override;
    ReturnValue Request_GetEngineStates(long session_id) override;
    ReturnValue Request_RTCAwakeVuc(Req_RTCAwakeVuc* req, long session_id) override;
    ReturnValue Request_DelayEngineRunningTime(Req_DelayEngineRunngTime* req, long session_id) override;
    ReturnValue Request_RemoteClimateOperate(Req_OperateRMTClimate* req, long session_id) override;
    ReturnValue Request_RMTSeatHeat(vc::Req_OperateRMTSeatHeat* req, long session_id) override;
    ReturnValue Request_RMTSeatVentilation(vc::Req_OperateRMTSeatVenti* req, long session_id) override;
    ReturnValue Request_RMTSteerWhlHeat(vc::Req_OperateRMTSteerWhlHeat_S* req, long session_id) override;
    ReturnValue Request_ParkingClimateOperate(vc::Req_ParkingClimateOper_S* req, long session_id) override;
    //end uia93888
    ReturnValue Request_GetDoorState(long session_id) override;
    
    // Test
    ReturnValue Request_GetDSParameter(ReqGetDSParameter&& req, long session_id) override;
    ReturnValue Request_VGMTest(ReqVGMTestMsg *req, long session_id) override;
    ReturnValue Request_IHUTest(ReqIHUTestMsg *req, long session_id) override;
    ReturnValue Event_Test_FromVC(EventTestFromVC *ev, long session_id) override;
    ReturnValue Request_Test_ToggleUsageMode(long session_id) override;
    ReturnValue Request_Test_ToggleCarMode(long session_id) override;
    ReturnValue Request_Test_CrashState(long session_id) override;
    ReturnValue Request_Test_OhcState(long session_id) override;
    ReturnValue Request_Test_SetFakeValuesDefault(long session_id) override;
    ReturnValue Request_Test_SetFakeValueInt(ReqTestSetFakeValueInt *req, long session_id) override;
    ReturnValue Request_Test_SetFakeValueStr(ReqTestSetFakeValueStr *req, long session_id) override;
    ReturnValue Request_Test_SetLocalConfig(ReqTestSetLocalConfig *req, long session_id) override;
 
    // IBLE
    ReturnValue Request_BleDisconnect(ReqBleDisconnect *req, long session_id) override;
    ReturnValue Request_BleDisconnectAll(long session_id) override;
    ReturnValue Request_BleGenerateBdak(long session_id) override;
    ReturnValue Request_BleGetBdak(long session_id) override;
    ReturnValue Request_BleConfigDatacomService(ReqBleCfgDataCommService *req, long session_id) override;
    ReturnValue Request_BleConfigIbeacon(ReqBleCfgIbeacon *req, long session_id) override;
    ReturnValue Request_BleConfigGenericAccess(ReqBleCfgGenAcc *req, long session_id) override;
    ReturnValue Request_BleStartPairingService(ReqBleStartPairService *req, long session_id) override;
    ReturnValue Request_BleStopPairingService(long session_id) override;
    ReturnValue Request_BleWakeupResponse(long session_id) override;
    ReturnValue Request_BleDataAccessAuthConfirmed(ReqBleDataAccAuthCon *req, long session_id) override;
    ReturnValue Request_BleStartAdvertising(ReqBleStartAdvertising *req, long session_id) override;
    ReturnValue Request_BleStopAdvertising(long session_id) override;

    // IDiagnostics
    ReturnValue Request_SwitchToFlashBootloader(long session_id) override;
    ReturnValue Request_LastBackupBatteryState(long session_id) override;
    ReturnValue Request_GetVucSwBuild(long session_id) override;
    ReturnValue Request_GetBleSwBuild(long session_id) override;
    ReturnValue Request_VucSwVersion(long session_id) override;
    ReturnValue Request_BleSwVersion(long session_id) override;
    ReturnValue Request_DtcEntries(ReqDtcEntries *req, long session_id) override;
    ReturnValue Request_DtcCtlSettings(ReqDtcCtlSettings *req, long session_id) override;
    ReturnValue Request_GetTcamHwVersion(long session_id) override;
    ReturnValue Request_GetRfVersion(long session_id) override;
    ReturnValue Request_GetFblSwBuild(long session_id) override;
    ReturnValue Request_CarConfigFactoryRestore(long session_id) override;
    ReturnValue Request_VucTraceConfig(ReqVucTraceConfig *req, long session_id) override;
    ReturnValue Request_VucTraceControl(ReqVucTraceControl *req, long session_id) override;
    ReturnValue Request_GetDIDGlobalSnapshotData(long session_id) override;
    ReturnValue Request_DTCsDetected(ReqDTCsDetected *req, long session_id) override;
    ReturnValue Request_RolloverValue(ReqRolloverValue *req, long session_id) override;
    ReturnValue Request_SetRolloverCfg(ReqSetRolloverCfg *req, long session_id) override;
};

}; // namespace vc

#endif // VEHICLE_COMM_CLIENT_HPP

