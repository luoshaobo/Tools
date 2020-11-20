#include "signals/geely/gly_vds_rvs_signal.h"
#include "voc_framework/signals/signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

std::shared_ptr<fsm::Signal> GlyVdsRvsSignal::CreateGlyVdsRvsSignal(fsm::VdServiceTransactionId& transaction_id,void* vdsService)
{
    GlyVdsRvsSignal* signal = new GlyVdsRvsSignal(transaction_id, vdsService);
    
    return std::shared_ptr<GlyVdsRvsSignal>(signal);
}

GlyVdsRvsSignal::GlyVdsRvsSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceRvs)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS %s\n", __FUNCTION__);

    m_vdMsg = std::make_shared<asn_wrapper::VDServiceRequest>();
    if (m_vdMsg == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                               "%s(), error, new m_vdMsg failed ", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
                               "%s(),  m_vdMsg = %0x ", __FUNCTION__, m_vdMsg.get());
    }
    m_EncodeAdapt = std::make_shared<fsm::RvsSignalAdapt>(m_vdMsg.get());
    if (m_EncodeAdapt == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                               "%s___, error, new m_EncodeAdapt failed ", __FUNCTION__);
    }
    bool result = UnpackPayload(vdsService);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,unpack RVS tsp request payload success!!!!!\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,unpack RVS tsp request payload fail!!!!!\n", __FUNCTION__);
    }
}

GlyVdsRvsSignal::~GlyVdsRvsSignal ()
{

}

bool GlyVdsRvsSignal::GetInternalPayload()
{				   
    return true;
}

bool GlyVdsRvsSignal::UnpackPayload(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS=====>%s\n", __FUNCTION__);

    m_rvsRequest = std::make_shared<RvsRequest_t>();
    if(m_rvsRequest == 0)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "[RVS] %s(), (m_rvsRequest == 0) \n", __FUNCTION__);
        return false;
    }

    if(vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s(), vdServiceRequest is NULL\n", __FUNCTION__);
        return false;
    }

    bool bRetVal = false;

    fsm::RvsSignalAdapt adapt(vdServiceRequest);
    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    m_rvsRequest = std::dynamic_pointer_cast<RvsRequest_t>(appData);
    if( m_rvsRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,"%s(): m_rvsRequest == nullptr", __FUNCTION__);
        return false;
    }
    else
    {
        bRetVal = true;
    
    }
	

    return bRetVal;
}

bool GlyVdsRvsSignal::SetHeadPayload()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    Timestamp now;
    
    VdmSignal::SetHeadPayload(m_rvsHeader.header);

    m_rvsHeader.header.eventId_presence = true;
    m_rvsHeader.header.creatorId_presence = true;
    m_rvsHeader.header.messageTTL_presence = true;
    m_rvsHeader.header.requestType_presence = true;
    m_rvsHeader.header.ackRequired_presence = true;

    //m_vdsResponse->header->requestid = m_vdsService->header->requestid;
    
    m_rvsHeader.header.timeStampSeconds = (now.epochMicroseconds()/1000000);
    m_rvsHeader.header.timeStampMilliseconds= (now.epochMicroseconds()%1000000)/1000;
     
    m_rvsHeader.header.eventIdMilliseconds_presence = true;
    m_rvsHeader.header.eventIdSeconds = (now.epochMicroseconds()/1000000);
    m_rvsHeader.header.eventIdMilliseconds= (now.epochMicroseconds()%1000000)/1000;

    m_rvsHeader.header.creatorId = 1;
    m_rvsHeader.header.messageTTL = 720;
    m_rvsHeader.header.ackRequired = false;

    return true;
}

void *GlyVdsRvsSignal::GetPackPayload()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RVS %s\n", __FUNCTION__);
    return (void *)NULL;
}

bool GlyVdsRvsSignal::SetBodyPayload()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetBodyPayload();
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetBodyPayload failed \n", __FUNCTION__);
    }
    return bRet;
}


bool GlyVdsRvsSignal::SetLockStatusPayload(const fsm::LockStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetLockStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}


bool GlyVdsRvsSignal::SetMaintenanceStatusPayload(const fsm::MaintenanceStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetMaintenanceStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetRunningStatusPayload(const fsm::RunningStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetRunningStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetClimateStatusPayload(const fsm::ClimateStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetClimateStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetPollutionStatusPayload(const fsm::PollutionStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetPollutionStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetElectricStatusPayload(const fsm::ElectricStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetElectricStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetBasicVehicleStatusPayload(const fsm::BasicVehicleStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetBasicVehicleStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetAdditionalStatusPayload(const fsm::AdditionalStatus_Data &payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAdditionalStatusPayload(payload);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, RVS Adapt Encode Failed\n", __FUNCTION__);
    }
    return true;
}

bool GlyVdsRvsSignal::SetBsPayload(Rvs_BS_t bs)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetBsPayload(bs);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetBsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetTsPayload(Rvs_TS_t ts)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetTsPayload(ts);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetTsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsCiPayload(Rvs_AS_CI_t as_ci)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsCiPayload(as_ci);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsCiPayload failed \n", __FUNCTION__);
    }
    
    return true;
}

bool GlyVdsRvsSignal::SetAsDsPayload(Rvs_AS_DS_t as_ds)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsDsPayload(as_ds);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsDsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsMsPayload(Rvs_AS_MS_t as_ms)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsMsPayload(as_ms);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsMsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsRsPayload(Rvs_AS_RS_t as_rs)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsRsPayload(as_rs);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsRsPayload failed \n", __FUNCTION__);
    }

    return true;
}    

bool GlyVdsRvsSignal::SetAsCsPayload(Rvs_AS_CS_t as_cs)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsCsPayload(as_cs);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsCsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsDbPayload(Rvs_AS_DB_t as_db)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsDbPayload(as_db);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsDbPayload failed \n", __FUNCTION__);
    }
      
    return true;
}

bool GlyVdsRvsSignal::SetAsEsPayload(Rvs_AS_ES_t as_es)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsEsPayload(as_es);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsEsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsPsPayload(Rvs_AS_PS_t as_ps)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsPsPayload(as_ps);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsPsPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsEwPayload(Rvs_AS_EW_t as_ew)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsEwPayload(as_ew);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsEwPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsCcPayload(Rvs_AS_CC_t as_cc)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsCcPayload(as_cc);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsCcPayload failed \n", __FUNCTION__);
    }

    return true;
}

bool GlyVdsRvsSignal::SetAsSignalsKeyPayload(Rvs_AS_SIGNALS_KEY_t as_signals_key)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RVS::%s\n", __FUNCTION__);

    bool bRet = false;
    bRet = m_EncodeAdapt->SetAsSignalsKeyPayload(as_signals_key);
    if(bRet == false)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, 
                "RVS::%s, call m_EncodeAdapt->SetAsCcPayload failed \n", __FUNCTION__);
    }

    return true;
}

void * GlyVdsRvsSignal::PackGeelyAsn()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "[RVS]%s() entered \n", 
                    __FUNCTION__);
    //lijing-test, use request message header
    bool result = m_EncodeAdapt->PackService(*m_rvsRequest);  
    if(!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s(), m_EncodeAdapt->PackService(*m_rvsRequest).\n", 
                __FUNCTION__);
        return nullptr;
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
                       "%s(),  m_vdMsg = %0x ", __FUNCTION__, m_vdMsg.get());

    return m_vdMsg.get();
}


}
