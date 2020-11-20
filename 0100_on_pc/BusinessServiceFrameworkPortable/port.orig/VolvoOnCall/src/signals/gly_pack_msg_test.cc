#include <memory>
#include "signals/gly_pack_msg_test.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/vdmsg_signal_types.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);


t_gly_pack_msg_test::t_gly_pack_msg_test(const fsm::TransactionId& transaction_id): fsm::Signal(transaction_id)
{
    m_vdmsg = nullptr;
}

t_gly_pack_msg_test::~t_gly_pack_msg_test()
{
    if(m_vdmsg)
    {
        delete m_vdmsg;
        m_vdmsg = NULL;
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                   "%s(): freeed vdmsg", __FUNCTION__);

}

void *t_gly_pack_msg_test::GetPackPayload()
{
    // int ret=0;
    if(m_vdmsg)
    {
        delete m_vdmsg;
        m_vdmsg = NULL;
    }
    VDServiceRequest * m_vdmsg = new VDServiceRequest();
    if (!m_vdmsg)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR,
                       "m_vdmsg is null");
        return NULL;
    }
    m_vdmsg->header = new RequestHeader();
    m_vdmsg->header->requestid = 76;
    m_vdmsg->header->timeStamp = new TimeStamp();

    m_vdmsg->optional.setPresence(asn_body);
    m_vdmsg->body = new RequestBody();

    m_vdmsg->body->optional.setPresence(asn_serviceId);
    m_vdmsg->body->serviceId = 77;

    m_vdmsg->body->optional.setPresence(asn_serviceData);
    m_vdmsg->body->serviceData = new ServiceData();

 //   m_vdmsg->body->serviceData->optional.setPresence(asn_serviceParameters);
 //   m_vdmsg->body->serviceData->serviceParameters = new asnList();

    m_vdmsg->body->serviceData->optional.setPresence(asn_serviceResult);
    m_vdmsg->body->serviceData->serviceResult = new ServiceResult();

    m_vdmsg->body->serviceData->serviceResult->optional.setPresence(asn_error);
    m_vdmsg->body->serviceData->serviceResult->error = new Error();
    m_vdmsg->body->serviceData->serviceResult->error->code = 8;

    m_vdmsg->body->serviceData->optional.setPresence(asn_ServiceData__ota);
    m_vdmsg->body->serviceData->ota = new OTA();

    m_vdmsg->body->serviceData->ota->optional.setPresence(asn_swModel);
    std::string temp = "test_swModel_val";
    m_vdmsg->body->serviceData->ota->swModel.copyOctetString(temp.length(), (asnbyte *)temp.c_str());
        
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                       "%s() ,111", __FUNCTION__);

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,
                       "%s() , return asn, m_vdmsg = %d", __FUNCTION__, (void *)m_vdmsg);
    return (void *)m_vdmsg;
}

fsm::Signal * t_gly_pack_msg_test::GetSignal()
{
    return this;
}


