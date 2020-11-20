#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_sources/signal_receiver_interface.h"

namespace fsm
{

VocmoSignalSource::VocmoSignalSource() {}

VocmoSignalSource::~VocmoSignalSource(){}

bool VocmoSignalSource::GetSimCurrentProfile(MTAData_t& mta)
{
  mta.msisdn = "18616351222";
  mta.imsi = "460018319704073";
  mta.icc_id = "89860118795910140735";

  return true;
}

bool VocmoSignalSource::GetNadIfNetworkSignalQuality(MTAData_t& mta)
{
  mta.mobile_network = "4G";
  return true;
}

bool VocmoSignalSource::GeelySendMessage(std::shared_ptr<fsm::SignalPack> signal, bool sms, int priority)
{
 return true;
}

void VocmoSignalSource::RegisterSignalReceiver(fsm::SignalReceiverInterface& recv)
{
}

void VocmoSignalSource::UnregisterSignalReceiver(fsm::SignalReceiverInterface& recv)
{
}

void VocmoSignalSource::ProcessSignal(std::shared_ptr<Signal> signal)
{
}

}
