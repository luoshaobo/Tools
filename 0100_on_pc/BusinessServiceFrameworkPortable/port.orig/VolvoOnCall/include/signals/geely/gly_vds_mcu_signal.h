///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_remote_config_signal.h
//	geely remote_config signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		zhouyou
// @Init date	11-Jan-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_MCU_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_MCU_SIGNAL_H_

#include "signals/signal_types.h"
#include "voc_framework/signals/signal.h"
#include "voc_framework/signals/signal_pack.h"
#include "voc_framework/signals/vdmsg_signal.h"
#include "voc_framework/signals/vdmsg_signal_types.h"
#include "voc_framework/signal_adapt/app_data.h"

#include <memory>
#include <string>

class SignalAdaptBase;


namespace volvo_on_call
{

#define MCU_BUF_SIZE      256

class GlyVdsMcuSignal: public fsm::VdmSignal, public fsm::SignalPack
{
     public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    GlyVdsMcuSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    ////////////////////////////////////////////////////////////
    // @brief :create mcu signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    static std::shared_ptr<fsm::Signal> CreateGlyVdsMcuSignal(fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    ~GlyVdsMcuSignal ();

    std::string ToString() {return "GlyVdsMcuSignal"; }
    
    ////////////////////////////////////////////////////////////
    // @brief :pack McuServiceResult_t into ASN1C structure.
    // @param[in]  response, McuServiceResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(McuServiceResult_t response);
    

    ////////////////////////////////////////////////////////////
    // @brief :pack McuConfiguration_t into ASN1C structure.
    // @param[in]  response, McuConfiguration_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool SetConfigurationPayload(McuConfiguration_t response);
    
    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool SetHeadPayload();
    
    ////////////////////////////////////////////////////////////
    // @brief :get packed ASN1C structure.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    void *GetPackPayload()
    {
	return (void *)NULL;
    }

    virtual int PackGeelyAsn(void *vdServiceRequest);

    fsm::Signal *GetSignal()
    {
	    return this;
    }

    ////////////////////////////////////////////////////////////
    // @brief :get unpacked ASN1C structure.
    // @return     unpacked structure
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    std::shared_ptr<McuRequest_t> GetRequestPayload()
    {
        return m_mcuRequest;
    }

 private:
    ////////////////////////////////////////////////////////////
    // @brief :unpack request ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool UnpackPayload(void * vdServiceRequest);
    
    ////////////////////////////////////////////////////////////
    // @brief :unpack configuration ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia/zhoushangjun, 11-Jan-2019
    ////////////////////////////////////////////////////////////
    bool UnpackConfigurationPayload(Configuration * configuration);


private:
    
    std::shared_ptr<McuServiceResult_t> m_mcuServiceResult;
    std::shared_ptr<McuRequest_t> m_mcuRequest;
     
};

   

}


#endif





