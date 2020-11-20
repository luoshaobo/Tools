#include "simulator_common.h"
#include "vc_simulator.h"

using namespace bsfwk;

namespace vc {
    namespace simulator {
        class VcGeneralSimulator : public IGeneral {
        public:
            // Requests
            virtual ReturnValue Request_LockDoor(ReqDoorLockUnlock *req, long session_id);
            virtual ReturnValue Request_HornAndLight(ReqHornNLight *req, long session_id);
            virtual ReturnValue Request_AntennaSwitch(ReqAntennaSwitch *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_LanLinkTrigger(ReqLanLinkTrigger *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_AdcValue(ReqAdcValue *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_VINNumber(long session_id) { return RET_OK; }
            virtual ReturnValue Request_CarUsageMode(long session_id);
            virtual ReturnValue Request_CarMode(long session_id);
            virtual ReturnValue Request_ElectEnergyLevel(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetWifiData(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetPSIMAvailability(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetFuelType(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetPropulsionType(long session_id) { return RET_OK; }
            virtual ReturnValue Request_IpWakeup(ReqIpWakeup *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_SetOhcLed(ReqSetOhcLed *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetVucPowerMode(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetVucWakeupReason(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetOhcBtnState(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetNumberOfDoors(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetSteeringWheelPosistion(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetFuelTankVolume(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetAssistanceServices(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetAutonomousDrive(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetCombinedInstrument(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetTelematicModule(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetConnectivity(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetConnectedServiceBooking(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetBeltInformation(long session_id) { return RET_OK; }
            virtual ReturnValue Request_SendPosnFromSatltCon(ReqSendPosnFromSatltCon req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetDiagErrReport(long session_id) { return RET_OK; }
            virtual ReturnValue Request_PSIMData(ReqPSIMData *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_ThermalMitigation(ReqThermalMitigataion *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetCarCfgTheftNotification(long session_id);
            virtual ReturnValue Request_GetCarCfgStolenVehicleTracking(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetGNSSReceiver(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetDigitalVideoRecorder(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetDriverAlertControl(long session_id) { return RET_OK; }
#if 1 // nieyj
            virtual ReturnValue Request_WindowsCtrl(ReqWinCtrl *req, long session_id);
            virtual ReturnValue Request_SunRoofAndCurtCtrl(ReqRoofCurtCtrl *req, long session_id);
            virtual ReturnValue Request_WinVentilation(ReqWinVentilation *req, long session_id);
            virtual ReturnValue Request_PM25Enquire(long session_id);
            virtual ReturnValue Request_VFCActivate(ReqVFCActivate *req, long session_id);
#endif
            //uia93888 remote start engine
            virtual ReturnValue Request_SendRMTEngine(Req_RMTEngine* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetRMTStatus(long session_id);
            virtual ReturnValue Request_GetEngineStates(long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetDoorState(long session_id) { return RET_OK; }
            virtual ReturnValue Request_RTCAwakeVuc(Req_RTCAwakeVuc* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_DelayEngineRunningTime(Req_DelayEngineRunngTime* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_RemoteClimateOperate(Req_OperateRMTClimate* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_RMTSeatHeat(vc::Req_OperateRMTSeatHeat* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_RMTSeatVentilation(vc::Req_OperateRMTSeatVenti* req, long session_id) { return RET_OK; }
            //end uia93888

            // Test
            virtual ReturnValue Request_VucTraceConfig(ReqVucTraceConfig *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_VucTraceControl(ReqVucTraceControl *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_GetDSParameter(ReqGetDSParameter&& req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_VGMTest(ReqVGMTestMsg *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_IHUTest(ReqIHUTestMsg *req, long session_id) { return RET_OK; }
            virtual ReturnValue Event_Test_FromVC(EventTestFromVC *ev, long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_ToggleUsageMode(long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_ToggleCarMode(long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_CrashState(long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_OhcState(long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_SetFakeValuesDefault(long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_SetFakeValueInt(ReqTestSetFakeValueInt *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_SetFakeValueStr(ReqTestSetFakeValueStr *req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_Test_SetLocalConfig(ReqTestSetLocalConfig *req, long session_id) { return RET_OK; }
            
            virtual ReturnValue Request_GetApprovedCellularStandards(long session_id) { return RET_OK; }
            virtual ReturnValue Request_RmtEngineSecurityRespose(Req_RMTEngineSecurityResponse* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_RMTSteerWhlHeat(vc::Req_OperateRMTSteerWhlHeat_S* req, long session_id) { return RET_OK; }
            virtual ReturnValue Request_ParkingClimateOperate(vc::Req_ParkingClimateOper_S* req, long session_id) { return RET_OK; }

        private:
            void Request_GetCarCfgTheftNotification_Response(long session_id);
            void Request_CarUsageMode_Response(long session_id);
            void Request_CarMode_Response(long session_id);
            void Request_GetRMTStatus_Response(long session_id);
            void Request_VFCActivate_Response(ReqVFCActivate req, long session_id);
            void Request_LockDoor_Response(ReqDoorLockUnlock req, long session_id);
            void Request_HornAndLight_Response(ReqHornNLight req, long session_id);
            void Request_PM25Enquire_Response(long session_id);
            void Request_WindowsCtrl_Response(ReqWinCtrl req, long session_id);
        };

        ReturnValue VcGeneralSimulator::Request_GetCarCfgTheftNotification(long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_GetCarCfgTheftNotification_Response, this, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_GetCarCfgTheftNotification_Response(long session_id)
        {
            msleep(GetRandom(1, 3000));

            ResGetTheftNotification res{ WITH_THEFT_NOTIFCATION, CARCONF_VALID };
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::GetTheftNotificationSignal> signal = std::make_shared<fsm::GetTheftNotificationSignal>(fsm::Signal::kCarUsageMode, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_CarUsageMode(long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_CarUsageMode_Response, this, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_CarUsageMode_Response(long session_id)
        {
            msleep(GetRandom(1, 3000));

            ResCarUsageMode res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::CarUsageModeSignal> signal = std::make_shared<fsm::CarUsageModeSignal>(fsm::Signal::kCarUsageMode, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_CarMode(long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_CarMode_Response, this, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_CarMode_Response(long session_id)
        {
			msleep(GetRandom(1, 3000));

            ResCarMode res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::CarModeSignal> signal = std::make_shared<fsm::CarModeSignal>(fsm::Signal::kCarMode, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_GetRMTStatus(long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_GetRMTStatus_Response, this, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_GetRMTStatus_Response(long session_id)
        {
			msleep(GetRandom(1, 3000));

            ResRMTStatus res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::RMTCarStatesSignal> signal = std::make_shared<fsm::RMTCarStatesSignal>(fsm::Signal::kRMTCarStatesSignal, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_VFCActivate(ReqVFCActivate *req, long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_VFCActivate_Response, this, *req, session_id));

            if ((GetRandom(0, 6000) % 2) == 0) {
                return RET_OK;
            } else {
                return RET_ERR_INTERNAL;
            }
        }

        void VcGeneralSimulator::Request_VFCActivate_Response(ReqVFCActivate req, long session_id)
        {
			msleep(GetRandom(2000, 6000));

            ResVFCActivate res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::VFCActivateSignal> signal = std::make_shared<fsm::VFCActivateSignal>(fsm::Signal::kVFCActivateSignal, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_LockDoor(ReqDoorLockUnlock *req, long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_LockDoor_Response, this, *req, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_LockDoor_Response(ReqDoorLockUnlock req, long session_id)
        {
			msleep(GetRandom(1, 3000));

            EventDoorLockState res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::DoorLockStatusSignal> signal = std::make_shared<fsm::DoorLockStatusSignal>(fsm::Signal::kDoorLockStatusSignal, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_HornAndLight(ReqHornNLight *req, long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_HornAndLight_Response, this, *req, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_HornAndLight_Response(ReqHornNLight req, long session_id)
        {
            msleep(GetRandom(1, 3000));

            ResHornNLight res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::HornNLightSignal> signal = std::make_shared<fsm::HornNLightSignal>(fsm::Signal::kHornNLight, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_PM25Enquire(long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_PM25Enquire_Response, this, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_PM25Enquire_Response(long session_id)
        {
			msleep(GetRandom(1, 3000));

            EventPMLevelState res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::PMLvlStatusSignal> signal = std::make_shared<fsm::PMLvlStatusSignal>(fsm::Signal::kRMTCarStatesSignal, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_WindowsCtrl(ReqWinCtrl *req, long session_id)
        {
            really_async(std::bind(&VcGeneralSimulator::Request_WindowsCtrl_Response, this, *req, session_id));
            return RET_OK;
        }

        void VcGeneralSimulator::Request_WindowsCtrl_Response(ReqWinCtrl req, long session_id)
        {
			msleep(GetRandom(1, 3000));

            EventWinOpenState res{};
            fsm::VehicleCommTransactionId transaction_id(session_id);
            std::shared_ptr<fsm::WinOpenStatusSignal> signal = std::make_shared<fsm::WinOpenStatusSignal>(fsm::Signal::kWinOpenStatusSignal, transaction_id, res, RET_OK);
            PostSignalEvent(signal);
        }

        ReturnValue VcGeneralSimulator::Request_SunRoofAndCurtCtrl(ReqRoofCurtCtrl *req, long session_id)
        {
            return RET_OK;
        }

        ReturnValue VcGeneralSimulator::Request_WinVentilation(ReqWinVentilation *req, long session_id)
        {
            return RET_OK;
        }

        IGeneral& GetSimulatorGeneralInterface()
        {
            static VcGeneralSimulator vcGeneralSimulator;
            return vcGeneralSimulator;
        }

    } // namespace simulator {
} //namespace vc {
