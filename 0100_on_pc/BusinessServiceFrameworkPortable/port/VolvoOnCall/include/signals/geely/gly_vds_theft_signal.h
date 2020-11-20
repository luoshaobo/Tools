///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_theft_signal.h
// geely theft signal.

// @project         GLY_TCAM
// @subsystem   Application
// @author        Hu Tingting
// @Init date     3-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_THEFT_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_THEFT_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal_types.h"

//#include "asn.h"
#include "voc_framework/signals/vdmsg_signal.h"
//#include "voc_framework/signal_adapt/theft_signal_adapt.h"

#include <memory>

namespace volvo_on_call
{

typedef struct {
    long    TimeSeconds;
    long    TimeMilliseconds;
    long    activated;
} TheftUpload_t;

typedef struct {
    int    service_id;
    int    ack_flag;
} TheftAck_t;

class GlyVdsTheftSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:

    /************************************************************/
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    GlyVdsTheftSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);


    /************************************************************/
    // @brief :create theft signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    static std::shared_ptr<fsm::Signal> CreateGlyVdsTheftSignal (fsm::VdServiceTransactionId& transaction_id,void* vdsService);


    /************************************************************/
    // @brief :destruct signal.
    // @param[in]  none
    // @return
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    ~GlyVdsTheftSignal ();

    std::string ToString() {return "GlyVdsTheftSignal"; }


    /************************************************************/
    // @brief :pack TheftUpload_t into ASN1C structure. upload body msg.
    // @param[in]  response, TheftUpload_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool SetTheftPayload(TheftUpload_t response);
    TheftUpload_t GetTheftPayload();

    /************************************************************/
    // @brief :pack RequestHeader.upload head msg.
    // @return     True if successfully set, false otherwise
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool SetHeadPayload();


    /************************************************************/
    // @brief :get packed ASN1C structure. Default encode upload func.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    void *GetPackPayload()
    {
        return nullptr;
    }

    virtual int PackGeelyAsn(void *vdServiceRequest) override;

    fsm::Signal *GetSignal()
    {
        return this;
    }
    /************************************************************/
    // @brief :get unpacked ASN1C structure.used to return ack msg with own struct.
    // @return     unpacked structure
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    std::shared_ptr<TheftAck_t> GetRequestPayload()
    {
        return m_theftAck;
    }


 private:
    /************************************************************/
    // @brief :unpack ASN1C structure.Currently used to receive ack msg.
    // @return     True if successfully set, false otherwise
    // @author     Hu Tingting, 3-Sep-2018
    /************************************************************/
    bool UnpackPayload();

 private:
    //fsm asn1 signal VDServiceRequest
    //asn_wrapper::VDServiceRequest* m_vdsService;
    TheftUpload_t m_response;
   //change VDServiceRequest to own struct
    std::shared_ptr<TheftAck_t> m_theftAck;
   //store eventId
    TheftUpload_t m_eventId;

};


} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

