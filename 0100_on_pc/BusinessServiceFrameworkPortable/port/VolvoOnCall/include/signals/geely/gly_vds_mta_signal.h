///////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// @file gly_vds_mta_signal.h
// geely journal signal.

// @project       GLY_TCAM
// @subsystem     VolvoOnCall
// @author        Jianhui Li
// @Init date     17-Jan-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_MTA_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_MTA_SIGNAL_H_

#include "asn.h"
#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_sources/vocmo_signal_source.h"
#include "voc_framework/signal_adapt/app_data.h"

#include <memory>

namespace volvo_on_call
{
class GlyVdsMTASignal: public fsm::VdmSignal, public fsm::SignalPack
{
 public:

    /************************************************************/
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Li Jianhui, 15-Feb-2019
    /************************************************************/
    GlyVdsMTASignal (fsm::VdServiceTransactionId& transaction_id,void* vdServiceRequest);


    /************************************************************/
    // @brief :create journal signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author      Li Jianhui, 10-Feb-2019
    /************************************************************/
    static std::shared_ptr<fsm::Signal> CreateGlyVdsMTASignal (fsm::VdServiceTransactionId& transaction_id,void* vdServiceRequest);


    /************************************************************/
    // @brief :destruct signal.
    // @param[in]  none
    // @return
    // @author      Li Jianhui, 10-Feb-2019
    /************************************************************/
    ~GlyVdsMTASignal ();

    std::string ToString() {return "GlyVdsMTASignal"; }


    /************************************************************/
    // @brief :pack MTAData_t into ASN1C structure. upload body msg.
    // @param[in]  response, JournalData_t structure .
    // @return     True if successfully set, false otherwise
    // @author      Li Jianhui, 10-Feb-2019
    /************************************************************/
    bool SetMTAPayload(const fsm::MTAData_t& response);

    /************************************************************/
    // @brief :get packed ASN1C structure. Default encode upload func.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author      Li Jianhui, 10-Feb-2019
    /************************************************************/
    void *GetPackPayload()
    {
        return nullptr;
    }

    fsm::Signal *GetSignal()
    {
        return this;
    }

    virtual int PackGeelyAsn(void *vdServiceRequest);

 private:
    /************************************************************/
    // @brief :unpack ASN1C structure.Currently used to receive ack msg.
    // @return     True if successfully set, false otherwise
    // @author      Li Jianhui, 10-Feb-2019
    /************************************************************/
    bool UnpackPayload(void *vdServiceRequest);

 private:

    std::shared_ptr<fsm::MTAData_t> m_mtaData;
 };

} // namespace volvo_on_call

#endif //VOC_SIGNALS_GLY_VDS_MTA_SIGNAL_H_

