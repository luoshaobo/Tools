///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file gly_vds_svt_signal.h
//	geely svt signal.

// @project		GLY_TCAM
// @subsystem	Application
// @author		linhaixia
// @Init date	3-Sep-2018
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_SVT_SIGNAL_NEW_H_
#define VOC_SIGNALS_GLY_VDS_SVT_SIGNAL_NEW_H_

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

class GlyVdsSvtSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    GlyVdsSvtSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);


    ////////////////////////////////////////////////////////////
    // @brief :create svt signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    static std::shared_ptr<fsm::Signal> CreateGlyVdsSvtSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    ~GlyVdsSvtSignal ();
    
    std::string ToString() {return "GlyVdsSvtSignal"; }

    ////////////////////////////////////////////////////////////
    // @brief :pack SvtServiceResult_t into ASN1C structure.
    // @param[in]  response, SvtServiceResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SetResultPayload(SvtServiceResult_t response);
    

    ////////////////////////////////////////////////////////////
    // @brief :pack SvtTrackPoint_t into ASN1C structure.
    // @param[in]  response, SvtTrackPoint_t structure .
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SetTrackPointPayload(SvtTrackPoint_t response);
    
    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SetHeadPayload();
    
    
    ////////////////////////////////////////////////////////////
    // @brief :get packed ASN1C structure.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     linhaixia, 3-Sep-2018
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
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    std::shared_ptr<SvtRequest_t> GetRequestPayload()
    {
        return m_svtRequest;
    }


    bool getCheckedCarMode()
    {
        return m_checkedCarMode;
    }
    
    void setCheckedCarMode(bool b)
    {
        m_checkedCarMode = b;
    }
    
    bool getCheckedCarConfigSvt()
    {
        return m_checkedCarConfigSvt;
    }
    
    void setCheckedCarConfigSvt(bool b)
    {
        m_checkedCarConfigSvt = b;
    }
    
    bool getCarMode()
    {
        return m_carMode;
    }
    
    void setCarMode(bool b)
    {
        m_carMode = b;
    }
    
    bool getCarConfigSvt()
    {
        return m_carConfigSvt;
    }
    
    void setCarConfigSvt(bool b)
    {
        m_carConfigSvt = b;
    }
    
 private:
    ////////////////////////////////////////////////////////////
    // @brief :unpack ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool UnpackPayload(void * vdServiceRequest);

 private:
#if 0
    asn_wrapper::VDServiceRequest* m_vdsResponse;
#endif
    std::shared_ptr<SvtRequest_t> m_svtRequest;

    std::shared_ptr<SvtServiceResult_t> m_SvtServiceResult;
    std::shared_ptr<SvtTrackPoint_t> m_SvtTrackPoint;

    bool m_checkedCarMode = false;
    bool m_checkedCarConfigSvt = false;
    bool m_carMode = false;
    bool m_carConfigSvt = false;
    
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

/** \}    end of addtogroup */
