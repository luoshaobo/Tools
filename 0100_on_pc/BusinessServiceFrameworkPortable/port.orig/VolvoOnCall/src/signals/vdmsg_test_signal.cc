#include <memory>
#include "signals/vdmsg_test_signal.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/vdmsg_signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);


VDMsgTestSignal::VDMsgTestSignal(fsm::VdServiceTransactionId& transaction_id, VDServiceRequest  * vdmsg)
                                                                                                        : fsm::Signal::Signal(transaction_id, fsm::KVDServiceTest)
{
    my_signal_type = 0;
    my_reqId = 0;
    UnpackPayload(vdmsg);
}

VDMsgTestSignal::~VDMsgTestSignal()
{
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                       "%s(): freeed vdmsg", __FUNCTION__);

}

std::shared_ptr<fsm::Signal> VDMsgTestSignal::CreateVdmsgSignal(fsm::VdServiceTransactionId& transaction_id,VDServiceRequest  * vdmsg)                                                                                  
{

    VDMsgTestSignal* signal = new VDMsgTestSignal(transaction_id,vdmsg);

    delete vdmsg;

    return std::shared_ptr<VDMsgTestSignal> (signal);
}


std::string VDMsgTestSignal::ToString() 
{
    return "VDMsg Test Signal";
}


bool VDMsgTestSignal::UnpackPayload(VDServiceRequest  * vdmsg)
{
    my_reqId = vdmsg->header->requestid;
    my_signal_type = vdmsg->body->serviceId;
    return true;
}

void *VDMsgTestSignal::GetPackPayload()
{
#if 0

    // int ret=0;
    if(vdmsg)
    {
        asn_DEF_VDServiceRequest.free_struct(&asn_DEF_VDServiceRequest, vdmsg, 0);
        vdmsg = NULL;

    }

    vdmsg = (VDServiceRequest_t*) calloc(1, sizeof(VDServiceRequest_t));
    if (!vdmsg)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "vdmsg is null");
        return NULL;
    }
    vdmsg->header.requestid = 222;

    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "111");

    vdmsg->body = (RequestBody_t *)calloc(1, sizeof(RequestBody_t));
    if (!vdmsg->body)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "vdmsg->body is null");
        return NULL;
    }

    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "222");

    //serviceData
    vdmsg->body->serviceData = (ServiceData_t *)calloc(1, sizeof(ServiceData_t));
    if (!vdmsg->body->serviceData)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "vdmsg->body->serviceData is null");
        return NULL;
    }

    vdmsg->body->serviceId = (ServiceId_t *)calloc(1, sizeof(ServiceId_t));
    if (!vdmsg->body->serviceId)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "vdmsg->body->serviceId is null");
        return NULL;
    }
    *(vdmsg->body->serviceId) = 3;

    DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,                        "333");

    // ServiceParameter *tmp_serviceParameter = static_cast<ServiceParameter *>(calloc(1, sizeof(ServiceParameter)));
    // if (!tmp_serviceParameter)
    // {
    //     DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
    //                    "tmp_serviceParameter is null");
    //     return NULL;
    // }

    // DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,                        "444111222");


    // //set serviceParameters
    // vdmsg->body->serviceData->serviceParameters = static_cast<struct ServiceData_t::ServiceData__serviceParameters *>(calloc(1, sizeof(struct ServiceData_t::ServiceData__serviceParameters)));
    // if (!vdmsg->body->serviceData->serviceParameters)
    // {
    //     DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
    //                    "vdmsg->body->serviceData->serviceParameters is null");
    //     return NULL;
    // }

    // vdmsg->body->serviceData->serviceParameters->list.array = static_cast<ServiceParameter_t**>(calloc(1, sizeof(ServiceParameter_t*)));
    // if (!vdmsg->body->serviceData->serviceParameters->list.array)
    // {
    //     DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
    //                    "vdmsg->body->serviceData->serviceParameters->list.array is null");
    //     return NULL;
    // }

    // DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,                        "555");

    // OCTET_STRING_fromString(&(tmp_serviceParameter->key), "12345678901234567");
    // ret = ASN_SEQUENCE_ADD(&(vdmsg->body->serviceData->serviceParameters->list), tmp_serviceParameter);
    // if (ret!=0)
    // {
    //     DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
    //                    "ASN_SEQUENCE_ADD serviceParameters failed");
    //     return NULL;
    // }

    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO,
                       "VDMsgTestSignal::GetPackPayload___() , return vdmsg");
    return (void *)vdmsg;

#endif

    return NULL;

}











