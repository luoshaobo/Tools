///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_theft_signal.cc
// geely theft signal.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Hu Tingting
// @Init date     3-Sep-2018
///////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "timestamp.h"
#include "dlt/dlt.h"
#include "signals/signal_types.h"
#include "signals/geely/gly_vds_theft_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
//#include "voc_framework/signal_adapt/theft_signal_adapt.h"


DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

/************************************************************/
// @brief :create theft signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
std::shared_ptr<fsm::Signal> GlyVdsTheftSignal::CreateGlyVdsTheftSignal(fsm::VdServiceTransactionId& transaction_id,void* vdsService)
{
    GlyVdsTheftSignal* signal = new GlyVdsTheftSignal(transaction_id, vdsService);

    return std::shared_ptr<GlyVdsTheftSignal>(signal);
}


/************************************************************/
// @brief :create theft signal.
// @param[in]  VdServiceTransactionId, vds transactionid.
// @param[in]  vdsService, vds msg.
// @return
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
GlyVdsTheftSignal::GlyVdsTheftSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService):
                   fsm::VdmSignal(transaction_id, fsm::kVDServiceStn),
                   //m_vdsService((asn_wrapper::VDServiceRequest *)vdsService),
                   m_response()
{
//    bool result = UnpackPayload();
    bool result = 1;
    if(result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,theft: create theft signal success.\n", __FUNCTION__);
    }
    else
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s,theft: create theft signal fail.\n", __FUNCTION__);
    }
    memset(&m_eventId, 0, sizeof(TheftUpload_t));
}


/************************************************************/
// @brief :destruct signal.
// @param[in]  none
// @return
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
GlyVdsTheftSignal::~GlyVdsTheftSignal ()
{
     DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: call %s()\n", __FUNCTION__);

     //if (m_vdsService)
     //{
     //    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"theft: %s(): free m_vdsService", __FUNCTION__);
     //    delete m_vdsService;
     //}
#if 0
     if (m_eventId)
     {
         DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO,"theft: %s(): free m_eventId", __FUNCTION__);
         delete m_eventId;
     }
#endif
}


/************************************************************/
// @brief :unpack ASN1C structure.Currently used to receive ack msg.
// @return     True if successfully set, false otherwise
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool GlyVdsTheftSignal::UnpackPayload()
{

    //m_theftAck = std::make_shared<TheftAck_t>();

    //if(m_theftAck == 0)
    //    return false;

    //if(m_vdsService == nullptr)
    //    return false;

    //TheftSignalAdapt adapt(m_vdsService);
    //std::shared_ptr<AppDataBase> appData = adapt.UnpackService();

    //std::shared_ptr<TheftRequest_t> theftRequest = std::dynamic_pointer_cast<TheftRequest_t>(appData);

    //if(theftRequest == nullptr)
    //{
    //    DLT_LOG_STRINGF(dlt_voc,DLT_LOG_INFO, "%s(): theftRequest == nullptr", __FUNCTION__);
    //    return false;
    //}

    //m_theftAck->service_id = theftRequest->serviceId;

    return true;

}


/************************************************************/
// @brief :pack TheftUpload_t into ASN1C structure. upload body msg.
// @param[in]  response, TheftUpload_t structure.
// @return     True if successfully set, false otherwise
// @author     Hu Tingting, 3-Sep-2018
/************************************************************/
bool GlyVdsTheftSignal::SetTheftPayload(TheftUpload_t response)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "theft: GlyVdsTheftSignal::%s\n", __FUNCTION__);
    m_response = response;

    return true;

}

TheftUpload_t GlyVdsTheftSignal::GetTheftPayload()
{
    return m_response;
}

//////////////////////////////////////////////////////
// @brief : encode vds
// @param[in]  vdServiceRequest
// @return   int
// @author     Tan Chang, Mar 6, 2019
//////////////////////////////////////////////////////
int GlyVdsTheftSignal::PackGeelyAsn(void *vdServiceRequest)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s() entered \n", __FUNCTION__);
    //if (vdServiceRequest == NULL)
    //{
    //    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s() vdServiceRequest is null \n", __FUNCTION__);
    //    return -1;
    //}
    //TheftServiceResult_t notification = TheftServiceResult_t();
    //notification.header.isInitiator = true;
    //notification.seconds = m_response.TimeSeconds;
    //notification.milliseconds = m_response.TimeMilliseconds;
    //notification.activated = m_response.activated;

    //TheftSignalAdapt adapt(vdServiceRequest);
    //bool ret = adapt.SetResultPayload(notification);
    //return (ret == true? 0 : -1);

    return true;
}

} // namespace volvo_on_call
