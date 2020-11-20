///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_mta_signal.cc
// geely mta signal.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Li Jianhui
// @Init date     15-Feb-2019
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "dlt/dlt.h"

#include "voc_framework/signal_adapt/mta_signal_adapt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_mta_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

/************************************************************/
// @brief :create mta signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Li Jianhui, 15-Feb-2019
/************************************************************/
std::shared_ptr<fsm::Signal> GlyVdsMTASignal::CreateGlyVdsMTASignal(fsm::VdServiceTransactionId& transaction_id,void* vdServiceRequest)
{
    return std::make_shared<GlyVdsMTASignal>(transaction_id, vdServiceRequest);
}


/************************************************************/
// @brief :create mta signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Li Jianhui, 15-Feb-2019
/************************************************************/
GlyVdsMTASignal::GlyVdsMTASignal (fsm::VdServiceTransactionId& transaction_id,void* vdServiceRequest):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceMta)
{
    m_mtaData = nullptr;
    bool result = UnpackPayload(vdServiceRequest);
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,get MTA request payload success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,get MTA request payload fail.\n", __FUNCTION__);
    }
}


/************************************************************/
// @brief :destruct signal.
// @param[in]  none
// @return
// @author     Li Jianhui, 15-Feb-2019
/************************************************************/
GlyVdsMTASignal::~GlyVdsMTASignal()
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "GlyVdsMTASignal::%s\n", __FUNCTION__);
}


/************************************************************/
// @brief :unpack ASN1C structure.Currently used to receive ack msg.
// @return     True if successfully set, false otherwise
// @author     Li Jianhui, 15-Feb-2019
/************************************************************/
bool GlyVdsMTASignal::UnpackPayload(void *vdServiceRequest)
{
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "UnpackPayload vdServiceRequest.");
 
    MtaSignalAdapt adapt(vdServiceRequest);
    return true;
    std::shared_ptr<AppDataBase> appData = adapt.UnpackService();
    m_mtaData = std::dynamic_pointer_cast<fsm::MTAData_t>(appData);
    if( m_mtaData == nullptr)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"%s(): m_mtaData = nullptr", __FUNCTION__);
        return false;
    }
    return true;
}


/************************************************************/
// @brief :pack JournalData_t into ASN1C structure. upload body msg.
// @param[in]  response, MTAData_t structure.
// @return     True if successfully set, false otherwise
// @author     Li Jianhui, 15-Feb-2019
/************************************************************/
bool GlyVdsMTASignal::SetMTAPayload(const fsm::MTAData_t& data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s ", __FUNCTION__);

    m_mtaData = std::make_shared<fsm::MTAData_t>();
    *m_mtaData = data;
    m_mtaData->header.isInitiator = true;
    return true;
}

/************************************************************/
// @brief :pack data into to vdServiceRequest
// @param[in]   vdServiceRequest
// @return     0 if successfully set, -1 otherwise
// @author     Li Jianhui, 15-Feb-2019
/************************************************************/
int GlyVdsMTASignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s() entered \n", __FUNCTION__);
    if (vdServiceRequest == NULL)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() vdServiceRequest is null \n", __FUNCTION__);
        return -1;
    }

    if (m_mtaData != nullptr)
    {
        MtaSignalAdapt adapt((void *)vdServiceRequest);
        bool ret = adapt.SetMtaPayload(*m_mtaData);
        return (ret == true? 0 : -1);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s() error in else \n", __FUNCTION__);
        return -1;
    }
    
    return -1;
}

} // namespace volvo_on_call
