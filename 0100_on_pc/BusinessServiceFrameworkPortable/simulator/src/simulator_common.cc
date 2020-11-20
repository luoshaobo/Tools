#include "simulator_common.h"

using namespace bsfwk;

std::function<void(std::shared_ptr<fsm::Signal>)> g_PostSignalEvent_Driver = NULL;

void PostSignalEvent(std::shared_ptr<fsm::Signal> signal)
{
    if (g_PostSignalEvent_Driver != NULL) {
        g_PostSignalEvent_Driver(signal);
    }
}

int GetRandom(int nMin, int nMax)
{
	static std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(nMin, nMax);
	int nRandom = distribution(generator);

	return nRandom;
}

void temp__create_remote_control_signals()
{
    fsm::VdServiceTransactionId transaction_id;
    std::shared_ptr<volvo_on_call::GlyVdsRmcSignal> signal = std::make_shared<volvo_on_call::GlyVdsRmcSignal>(transaction_id, (void *)NULL, fsm::kVDServiceRdl);
}

void dummy__send_remote_control_response()
{
    fsm::RemoteCtrlBasicStatus status{};

    volvo_on_call::simulator::GeelySendMessage(status);
}

void dummy__create_vehicle_common_signals()
{
    vc::ResCarMode resCarMode{ vc::CAR_NORMAL };
    fsm::VehicleCommTransactionId transaction_id;
    new fsm::CarModeSignal(0, transaction_id, resCarMode, vc::RET_OK);
}

void dummy__create_vpom_signals()
{
    vpom::GNSSData gnsData{};
    fsm::VpomTransactionId vpomTransactionId;
    fsm::VpomGNSSPositionDataSignal::CreateVpomSignal(gnsData, vpomTransactionId, fsm::Signal::kGNSSPositionDataSignal);
}

void dummy__call_vpom_apis()
{
    VpomIPositioningService& service = vpom::simulator::GetSimulatorVpomPositioningService();

    {
        fsm::VpomTransactionId id;
        service.GNSSPositionDataRawRequest(id.GetId());
    }
}

void dummy__call_vehicle_common_apis()
{
    fsm::VehicleCommTransactionId id;
    vc::IGeneral& general = vc::simulator::GetSimulatorGeneralInterface();

    {
        general.Request_CarMode(id.GetId());
    }

    {
        general.Request_GetRMTStatus(id.GetId());
    }

    {
        general.Request_CarUsageMode(id.GetId());
    }

    {
        vc::ReqVFCActivate request{};
        general.Request_VFCActivate(&request, id.GetId());
    }

    {
        vc::ReqDoorLockUnlock request{};
        general.Request_LockDoor(&request, id.GetId());
    }

    {
        vc::ReqHornNLight request{};
        general.Request_HornAndLight(&request, id.GetId());
    }

    {
        general.Request_PM25Enquire(id.GetId());
    }

    {
        vc::ReqWinCtrl request{};
        general.Request_WindowsCtrl(&request, id.GetId());
    }

    {
        vc::ReqRoofCurtCtrl request{};
        general.Request_SunRoofAndCurtCtrl(&request, id.GetId());
    }

    {
        vc::ReqWinVentilation request{};
        general.Request_WinVentilation(&request, id.GetId());
    }
}
