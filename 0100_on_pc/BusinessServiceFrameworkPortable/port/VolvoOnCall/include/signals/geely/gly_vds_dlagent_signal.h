///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  file gly_vds_dlagent_signal.h
//  geely download agent signal.

// @project     GLY_TCAM
// @subsystem   application
// @author      ShenJiaojiao
// @Init date   27-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_SIGNALS_GLY_VDS_DLAGENT_SIGNAL_H_
#define VOC_SIGNALS_GLY_VDS_DLAGENT_SIGNAL_H_

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


class GlyVdsDLAgentSignal: public fsm::VdmSignal, public fsm::SignalPack
{

 public:

    ////////////////////////////////////////////////////////////
    // @brief :Constructs signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return
    // @author     Shen Jiaojiao, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    GlyVdsDLAgentSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    ////////////////////////////////////////////////////////////
    // @brief :create DLAgent signal.
    // @param[in]  VdServiceTransactionId, vds transactionid.
    // @param[in]  vdsService, vds msg.
    // @return     Signal shared ptr
    // @author     Shen Jiaojiao, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    static std::shared_ptr<fsm::Signal> CreateGlyVdsDLAgentSignal (fsm::VdServiceTransactionId& transaction_id, void * vdServiceRequest);

    ~GlyVdsDLAgentSignal ();

    std::string ToString() {return "GlyVdsDLAgentSignal"; }

    ////////////////////////////////////////////////////////////
    // @brief :get packed ASN1C structure.
    // @return     pointer to asn1c struct containing payload, caller must free
    // @author     Shen Jiaojiao, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    void *GetPackPayload()
    {
        return NULL;
    }

    fsm::Signal *GetSignal()
    {
        return this;
    }

    virtual int PackGeelyAsn(void *vdServiceRequest);

    ////////////////////////////////////////////////////////////
    // @brief :pack DLAgentServiceResult_t into ASN1C structure.
    // @param[in]  response, DLAgentServiceResult_t structure .
    // @return     True if successfully set, false otherwise
    // @author     Shen Jiaojiao, 27-Feb-2019
    ////////////////////////////////////////////////////////////
    bool SetAssignmentNotificationPayload(fsm::FotaAssignmentNotification_Data msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack AvailableAssignmentRequest_Data into ASN1C structure.
    // @param[in]  response, AvailableAssignmentRequest_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     Tian Yong, Mar 4, 2019
    ////////////////////////////////////////////////////////////
    bool SetAvailableAssignmentRequestPayload(fsm::AvailableAssignmentRequest_Data &msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack InstallationSummary_Data into ASN1C structure.
    // @param[in]  msg, InstallationSummary_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     Li Jianhui, 19-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetInstallationSummaryNotificationPayload(const fsm::InstallationSummary_Data& msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack EcuExceptionReport_Data into ASN1C structure.
    // @param[in]  msg, EcuExceptionReport_Data structure .
    // @return     True if successfully set, false otherwise
    // @author     Tan Chang, 29-Mar-2019
    ////////////////////////////////////////////////////////////
    bool SetExceptionReportNotificationPayload(const fsm::EcuExceptionReport_Data& msg);

    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool SetHeadPayload();

    std::shared_ptr<fsm::FOTA_Data> getFOTAData() { return m_fotaData; }

 private:
    ////////////////////////////////////////////////////////////
    // @brief :unpack ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 3-Sep-2018
    ////////////////////////////////////////////////////////////
    bool UnpackPayload(void * vdServiceRequest);

 private:
    std::shared_ptr<fsm::FOTA_Data> m_fotaData;
    std::shared_ptr<fsm::AvailableAssignmentRequest_Data> m_availableAssignmentRequestData;
    std::shared_ptr<fsm::FotaAssignmentNotification_Data> m_fotaAssignmentNotification;
    std::shared_ptr<fsm::InstallationSummary_Data> m_installationSummary;
    std::shared_ptr<fsm::EcuExceptionReport_Data> m_exceptionReport;
};


} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

/** \}    end of addtogroup */
