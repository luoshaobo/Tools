/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     bcc_cl_002_signal.h
 *  \brief    bcc_cl_002 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_BCC_CL_002_SIGNAL_H_
#define VOC_SIGNALS_BCC_CL_002_SIGNAL_H_


// fsm includes
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/signal.h"

// ans1c includes
#include "BCC-CL-002.h"

#include <memory>

namespace volvo_on_call
{


class BccCl002Signal: public fsm::Signal, public fsm::CCMEncodable, public fsm::ResponseInfoData
{

 public:

    static const std::string oid;

    ~BccCl002Signal ();

    /**
     * \brief Constructs empty signal from supplied data
     * \param[in] transaction_id  ccm header transactionId
     * \param[in] sequence_number ccm header sequenceNumber
     * \return shared pointer to created signal
     */
    static std::shared_ptr<BccCl002Signal> CreateBccCl002Signal(fsm::CCMTransactionId& transaction_id,
                                                                uint16_t sequence_number);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "BccCl002Signal"; };


 protected:

    /**
     * \brief Provide the bcc_cl_002 oid.
     * \return the oid
     */
    const char* GetOid ();

    /**
     * \brief Pack BCC-CL-002 into ASN1C structure.
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload ();

    /**
     * \brief Provides the ASN1C type descriptor for BCC-CL-002
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 10;

    /**
     * \brief Constructs empty signal
     */
    BccCl002Signal (fsm::CCMTransactionId& transaction_id,
                    uint16_t sequence_number);

    /**
     * \brief Constructs empty signal with reference to a ccm message
     */
    BccCl002Signal (ccm_Message* ccm,
                    fsm::TransactionId& transaction_id);
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_002_SIGNAL_H_

/** \}    end of addtogroup */
