///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_vds_rvdc_signal.cc
//	geely rvdc signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	27-Feb-2019
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_rvdc_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/rvdc_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

/************************************************************/
// @brief :create rvc signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
std::shared_ptr<fsm::Signal> GlyVdsRvdcSignal::CreateGlyVdsRvdcSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);
    GlyVdsRvdcSignal* signal = new GlyVdsRvdcSignal(transaction_id, vdServiceRequest);
    
    return std::shared_ptr<GlyVdsRvdcSignal>(signal);
}

/************************************************************/
// @brief :Constructs function.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
GlyVdsRvdcSignal::GlyVdsRvdcSignal (fsm::VdServiceTransactionId& transaction_id, void *vdServiceRequest)
    :fsm::VdmSignal(transaction_id, fsm::kVDServiceRvc)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,rvdc request payload success.", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,rvdc request payload fail.", __FUNCTION__);
    }
}

/************************************************************/
// @brief :destruct function.
// @param[in]  none
// @return
// @author     nieyujin, 27-Feb-2019
/************************************************************/
GlyVdsRvdcSignal::~GlyVdsRvdcSignal ()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);

}

/************************************************************/
// @brief :pack RVDC_Data into ASN1C structure. upload body msg.
// @param[in]  response, RVDC_Data structure .
// @return     True if successfully set, false otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
bool GlyVdsRvdcSignal::SetResultPayload(fsm::RVDC_Data payload)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);

    m_rvdcResponse = std::make_shared<fsm::RVDC_Data>();
    if (m_rvdcResponse == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() m_rvdcResponse is null \n", 
            __FUNCTION__);
        return false;
    }
    *m_rvdcResponse = payload;
    return true;
}

/************************************************************/
// @brief :pack RVDC_Data into ASN1C structure. upload body msg.
// @param[in]  response, RVDC_Data structure .
// @return     0 if successfully set, -1 otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
int GlyVdsRvdcSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);
    if (vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() vdServiceRequest is null", __FUNCTION__);
        return -1;
    }
    if(m_rvdcResponse != nullptr)
    {
        fsm::RvdcSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetRvdcPayload(*m_rvdcResponse);
        return (ret == true? 0 : -1);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() error in else", __FUNCTION__);
        return -1;
    }
    
    return -1;
}

/************************************************************/
// @brief :unpack ASN1C structure.
// @param[in]  ASN1C structur
// @return     True if successfully set, false otherwise
// @author     nieyujin, 27-Feb-2019
/************************************************************/
bool GlyVdsRvdcSignal::UnpackPayload(void * vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsRvdcSignal::%s", __FUNCTION__);
    
    fsm::RvdcSignalAdapt adapt(vdServiceRequest);
    
    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    m_rvdcRequest = std::dynamic_pointer_cast<fsm::RVDC_Data>(appData);

    if( m_rvdcRequest == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): m_rvdcRequest == nullptr", __FUNCTION__);
        return false;
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->requestid:%d. \n", m_rvdcRequest->header.requestid);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceId:%d. \n", m_rvdcRequest->serviceId);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->serviceCommand:%d. \n", m_rvdcRequest->serviceCommand);
    
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->appId:%d. \n", m_rvdcRequest->appId);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->maOrderId:%s. \n", m_rvdcRequest->maOrderId.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->timestamp:%s. \n", m_rvdcRequest->timestamp.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->vin:%s. \n", m_rvdcRequest->vin.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "UnpackPayload request->maBusinessType:%d. \n", m_rvdcRequest->maBusinessType);

    return true;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
