
#include "voc_framework/signals/vdmsg_signal.h"

#include "timestamp.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

VdmSignal::VdmSignal(VdServiceTransactionId&vdsId, const long signal_type):
                                    Signal::Signal(vdsId,signal_type)
{

}

VdmSignal::~VdmSignal()
{

    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s\n", __FUNCTION__);
}

// void VdmSignal::SetHeadPayload(VDServiceRequest* request)
// {
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s\n", __FUNCTION__);

    // if(request == nullptr)
    // {
        // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s request == nullptr. then new VDServiceRequest(). \n"
            // , __FUNCTION__);

        // request = new VDServiceRequest();
    // }

    // Timestamp now;
    // request->header = new RequestHeader();
    // request->header->timeStamp = new TimeStamp();
    // request->header->timeStamp->seconds = (now.epochMicroseconds()/1000000);

    // request->header->timeStamp->optional.setPresence(asn_milliseconds);
    // request->header->timeStamp->milliseconds= (now.epochMicroseconds()%1000000)/1000;

    // request->header->optional.setPresence(asn_creatorId);
    // request->header->creatorId = ecu;

    // request->header->optional.setPresence(asn_messageTTL);
    // request->header->messageTTL = 120;

    // request->header->optional.setPresence(asn_ackRequired);
    // request->header->ackRequired = false;

    // request->header->requestid = getRequestId();

    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s completed.\n", __FUNCTION__);

// }

// void VdmSignal::SetHeadPayload(asn_wrapper::VDServiceRequest* request)
// {
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s\n", __FUNCTION__);
    // if(request == nullptr)
    // {
        // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s request == nullptr. then new VDServiceRequest(). \n"
            // , __FUNCTION__);
        // request = new asn_wrapper::VDServiceRequest();
    // }
    // Timestamp now;
    // request->header = new asn_wrapper::RequestHeader();
    // request->header->timeStamp = new asn_wrapper::TimeStamp();
    // request->header->timeStamp->seconds = (now.epochMicroseconds()/1000000);
    // request->header->timeStamp->optional.setPresence(asn_milliseconds);
    // request->header->timeStamp->milliseconds= (now.epochMicroseconds()%1000000)/1000;
    // request->header->optional.setPresence(asn_creatorId);
    // request->header->creatorId = ecu;
    // request->header->optional.setPresence(asn_messageTTL);
    // request->header->messageTTL = 120;
    // request->header->optional.setPresence(asn_ackRequired);
    // request->header->ackRequired = false;
    // request->header->requestid = getRequestId();
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s completed.\n", __FUNCTION__);
// }

//int VdmSignal::SetHeadPayload(RequestHeader_Data &header)
//{
    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "VdmSignal::%s\n", __FUNCTION__);

    // Timestamp now;
    // header.timeStampSeconds = (now.epochMicroseconds()/1000000);

    // header.timeStampMilliseconds_presence = true;
    // header.timeStampMilliseconds = (now.epochMicroseconds()%1000000)/1000;

    // header.creatorId_presence = true;
    // header.creatorId = ecu;

    // header.messageTTL_presence = true;
    // header.messageTTL = 120;

    // header.ackRequired_presence = true;
    // header.ackRequired = false;

    // header.requestid = getRequestId();

    // DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO, "%s() completed.\n", __FUNCTION__);
    //return 0;
//}

}
