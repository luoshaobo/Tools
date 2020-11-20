///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_vds_svt_signal.h
//	geely svt signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	27-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_RVDC_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_RVDC_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/app_data.h"

#include <memory>

class SignalAdaptBase;

namespace volvo_on_call
{


class GlyVdsRvdcSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:
    /************************************************************/
    // @brief :create rvc signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    static std::shared_ptr<fsm::Signal> CreateGlyVdsRvdcSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    /************************************************************/
    // @brief :Constructs function.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    GlyVdsRvdcSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    /************************************************************/
    // @brief :destruct function.
    // @param[in]  none
    // @return
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    ~GlyVdsRvdcSignal ();

    std::string ToString() {return "GlyVdsRvdcSignal"; }
   
    /************************************************************/
    // @brief :pack RequestHeader.
    // @param[in]
    // @return     True if successfully set, false otherwise
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    bool SetHeadPayload();
    
    /************************************************************/
    // @brief :get packed ASN1C structure.
    // @param[in]
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    void *GetPackPayload()
    {
        return (void *)NULL;
    }

    /************************************************************/
    // @brief :pack RVDC_Data into ASN1C structure. upload body msg.
    // @param[in]  response, RVDC_Data structure .
    // @return     0 if successfully set, -1 otherwise
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    virtual int PackGeelyAsn(void *vdServiceRequest);

    fsm::Signal *GetSignal()
    {
	    return this;
    }

    /************************************************************/
    // @brief :get request payload structure.
    // @param[in]
    // @return     payload structure
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    std::shared_ptr<fsm::RVDC_Data> GetRequestPayload()
    {
        return m_rvdcRequest;
    }

    /************************************************************/
    // @brief :get response payload  structure.
    // @param[in]
    // @return     payload structure
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    std::shared_ptr<fsm::RVDC_Data> GetResponsePayload()
    {
        return m_rvdcResponse;
    }
    
    /************************************************************/
    // @brief :pack RVDC_Data into ASN1C structure. upload body msg.
    // @param[in]  response, RVDC_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    bool SetResultPayload(fsm::RVDC_Data payload);

 private:

    /************************************************************/
    // @brief :unpack ASN1C structure.
    // @param[in]  ASN1C structur
    // @return     True if successfully set, false otherwise
    // @author     nieyujin, 27-Feb-2019
    /************************************************************/
    bool UnpackPayload(void * vdServiceRequest);

 private:
    std::shared_ptr<fsm::RVDC_Data> m_rvdcRequest;
    std::shared_ptr<fsm::RVDC_Data> m_rvdcResponse;    
};


} // namespace volvo_on_call

#endif //VOC_SIGNALS_GLY_VDS_RVDC_SIGNAL_H_

/** \}    end of addtogroup */
