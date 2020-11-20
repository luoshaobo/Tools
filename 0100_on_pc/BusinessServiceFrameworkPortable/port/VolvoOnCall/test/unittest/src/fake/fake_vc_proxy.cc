#include "vc_proxy.hpp"
#include "fake_control.h"

namespace vc {

long MessageBase::unique_id_current = 0;
guint16 VehicleCommClient::client_id_ = -1;

VehicleCommClient::VehicleCommClient()
{
}

VehicleCommClient::~VehicleCommClient()
{
}

IGeneral& IVehicleComm::GetGeneralInterface()
{
    return *(IGeneral*)&IVehicleComm::GetInstance();
}

IDiagnostics& IVehicleComm::GetDiagnosticsInterface()
{
    return *(IDiagnostics*)&IVehicleComm::GetInstance();
}

IVehicleComm& IVehicleComm::GetInstance()
{
    static VehicleCommClient vc_;
    return *(IVehicleComm*)&vc_;
}

ReturnValue VehicleCommClient::EnableGeneralEvents(void) { return RET_OK; }
ReturnValue VehicleCommClient::DisableGeneralEvents(void) { return RET_OK; }
ReturnValue VehicleCommClient::EnableBLEEvents(void) { return RET_OK; }
ReturnValue VehicleCommClient::DisableBLEEvents(void) { return RET_OK; }
ReturnValue VehicleCommClient::EnableDiagnosticsEvents(void) { return RET_OK; }
ReturnValue VehicleCommClient::DisableDiagnosticsEvents(void){ return RET_OK; }
ReturnValue VehicleCommClient::Init(ResponseCallback response_callback, EventCallback event_callback) { return RET_OK; }

// IGeneral
ReturnValue VehicleCommClient::Request_LockDoor(ReqDoorLockUnlock *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_HornAndLight(ReqHornNLight *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_AntennaSwitch(ReqAntennaSwitch *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_LanLinkTrigger(ReqLanLinkTrigger *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_AdcValue(ReqAdcValue *req, long session_id) { return RET_OK; }

ReturnValue VehicleCommClient::Request_VINNumber(long session_id) 
{ 
   if( FakeVCProxyControl::Get_Request_VINNumber_Success_Or_Fail() )
   {
      return RET_OK; 
   }
   return RET_ERR_INTERNAL; // error as test
}

ReturnValue VehicleCommClient::Request_CarUsageMode(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_CarMode(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_ElectEnergyLevel(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetWifiData(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetPSIMAvailability(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetFuelType(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetPropulsionType(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_IpWakeup(ReqIpWakeup *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_SetOhcLed(ReqSetOhcLed *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetVucPowerMode(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetVucWakeupReason(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetOhcBtnState(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetNumberOfDoors(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetSteeringWheelPosistion(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetFuelTankVolume(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetAssistanceServices(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetTelematicModule(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetConnectivity(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetConnectedServiceBooking(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetBeltInformation(long session_id) { return RET_OK; }   
ReturnValue VehicleCommClient::Request_SendPosnFromSatltCon(ReqSendPosnFromSatltCon req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetDiagErrReport(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_PSIMData(ReqPSIMData *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_ThermalMitigation(ReqThermalMitigataion *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetCarCfgTheftNotification(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetCarCfgStolenVehicleTracking(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetGNSSReceiver(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_CarconfigParameterFaults(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_NetworkManagementHistory(long session_id) { return RET_OK; }
#if 1 // nieyj
ReturnValue VehicleCommClient::Request_WindowsCtrl(ReqWinCtrl *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_SunRoofAndCurtCtrl(ReqRoofCurtCtrl *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_WinVentilation(ReqWinVentilation *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_PM25Enquire(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_VFCActivate(ReqVFCActivate *req, long session_id) { return RET_OK; }
#endif
//uia93888 remote start engine
ReturnValue VehicleCommClient::Request_SendRMTEngine(Req_RMTEngine* req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetRMTStatus(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetEngineStates(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_RTCAwakeVuc(Req_RTCAwakeVuc* req, long session_id) { return RET_OK; }
//end uia93888
ReturnValue VehicleCommClient::Request_GetDoorState(long session_id) { return RET_OK; }
// Test
ReturnValue VehicleCommClient::Request_GetDSParameter(ReqGetDSParameter&& req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_VGMTest(ReqVGMTestMsg *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_IHUTest(ReqIHUTestMsg *req, long session_id) { return RET_OK; }
ReturnValue Event_Test_FromVC(EventTestFromVC *ev, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_ToggleUsageMode(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_ToggleCarMode(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_CrashState(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_OhcState(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_SetFakeValuesDefault(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_SetFakeValueInt(ReqTestSetFakeValueInt *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_SetFakeValueStr(ReqTestSetFakeValueStr *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_Test_SetLocalConfig(ReqTestSetLocalConfig *req, long session_id) { return RET_OK; }

// IBLE
ReturnValue VehicleCommClient::Request_BleDisconnect(ReqBleDisconnect *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleDisconnectAll(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleGenerateBdak(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleGetBdak(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleConfigDatacomService(ReqBleCfgDataCommService *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleConfigIbeacon(ReqBleCfgIbeacon *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleConfigGenericAccess(ReqBleCfgGenAcc *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleStartPairingService(ReqBleStartPairService *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleStopPairingService(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleWakeupResponse(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleDataAccessAuthConfirmed(ReqBleDataAccAuthCon *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleStartAdvertising(ReqBleStartAdvertising *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleStopAdvertising(long session_id) { return RET_OK; }

// IDiagnostics
ReturnValue VehicleCommClient::Request_SwitchToFlashBootloader(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_LastBackupBatteryState(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetVucSwBuild(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetBleSwBuild(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_VucSwVersion(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_BleSwVersion(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_DtcEntries(ReqDtcEntries *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_DtcCtlSettings(ReqDtcCtlSettings *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetTcamHwVersion(long session_id)
{ 
   if( FakeVCProxyControl::Get_Request_GetTcamHwVersion_Success_Or_Fail() )
   {
      return RET_OK; 
   }
   return RET_ERR_INTERNAL; // error as test

}

ReturnValue VehicleCommClient::Request_GetRfVersion(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetFblSwBuild(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_CarConfigFactoryRestore(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_VucTraceConfig(ReqVucTraceConfig *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_VucTraceControl(ReqVucTraceControl *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetDIDGlobalSnapshotData(long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_DTCsDetected(ReqDTCsDetected *req, long session_id) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetAutonomousDrive(long g) { return RET_OK; }
ReturnValue VehicleCommClient::Request_GetCombinedInstrument(long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_GetDigitalVideoRecorder(long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_GetDriverAlertControl(long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_DelayEngineRunningTime(Req_DelayEngineRunngTime*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_RemoteClimateOperate(Req_OperateRMTClimate*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_RMTSeatHeat(Req_OperateRMTSeatHeat*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_RMTSeatVentilation(Req_OperateRMTSeatVenti*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Event_Test_FromVC(EventTestFromVC*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_ProgramPreCheck(long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_RolloverValue(ReqRolloverValue*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_RmtEngineSecurityRespose(Req_RMTEngineSecurityResponse*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_SetRolloverCfg(ReqSetRolloverCfg*, long){ return RET_OK; }
ReturnValue VehicleCommClient::Request_GetApprovedCellularStandards(long session_id) { return RET_OK; }

}
