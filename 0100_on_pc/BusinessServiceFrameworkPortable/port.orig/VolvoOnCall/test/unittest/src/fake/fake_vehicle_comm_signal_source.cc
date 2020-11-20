#include "voc_framework/signal_sources/vehicle_comm_signal_source.h"
#include "vehiclecomm/vehicle_comm_interface.hpp"
#include "voc_framework/features/feature.h"
#include "voc_framework/signals/vehicle_comm_signal.h"
#include "voc_framework/signals/signal.h"

namespace fsm
{

VehicleCommSignalSource::VehicleCommSignalSource(): ivc_(vc::IVehicleComm::GetInstance()) {}

VehicleCommSignalSource::~VehicleCommSignalSource(){}

VehicleCommSignalSource& VehicleCommSignalSource::GetInstance()
{
    static VehicleCommSignalSource instance;
    return instance;
}

vc::IVehicleComm& VehicleCommSignalSource::GetVehicleCommClientObject()
{
  return ivc_;
}

void SignalSource::RegisterSignalReceiver(fsm::SignalReceiverInterface& recv)
{
}

void SignalSource::UnregisterSignalReceiver(fsm::SignalReceiverInterface& recv)
{
}

}

