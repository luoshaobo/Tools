#include "voc_framework/signals/vdmsg_signal.h"

namespace fsm
{
VdmSignal::VdmSignal(VdServiceTransactionId&vdsId, const long signal_type):
                                    Signal::Signal(vdsId,signal_type)
{
}

VdmSignal::~VdmSignal()
{  
}

void VdmSignal::SetHeadPayload(VDServiceRequest* request)
{
}

void VdmSignal::SetHeadPayload(asn_wrapper::VDServiceRequest* request)
{
}

} //end fsm

