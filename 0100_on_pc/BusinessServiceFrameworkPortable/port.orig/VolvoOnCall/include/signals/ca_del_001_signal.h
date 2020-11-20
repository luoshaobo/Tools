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
 *  \file     ca_del_001_signal.h
 *  \brief    CA_DEL_001 signal
 *  \author   Dariusz Szymczak
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_CA_DEL_001_SIGNAL_H_
#define VOC_SIGNALS_CA_DEL_001_SIGNAL_H_

// fsm includes
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/signal.h"

// voc includes
#include "signals/signal_types.h"

// ans1c includes
#include "CA-DEL-001.h"

namespace volvo_on_call
{

class CaDel001Signal: public fsm::Signal, public fsm::CCMEncodable
{

 public:

    /**
     * \brief fsm::Signal's OID
     */
    static const std::string oid;

    /**
     * \brief Destructor
     */
    ~CaDel001Signal ();

    /**
     * \brief Constructs empty signal from supplied data
     * \param[in] transaction_id  ccm header transactionId
     * \param[in] sequence_number ccm header sequenceNumber
     * \return shared pointer to created signal
     */
    static std::shared_ptr<CaDel001Signal> CreateCaDel001Signal(fsm::CCMTransactionId& transaction_id,
                                                                uint16_t sequence_number);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "CaDel001Signal"; };

    /**
     * \brief Provide the fsm/ccm encode flags to use when encoding, if any.
     *        Default impelementation will return kEncodeAsPayload,
     *        derived calss should override if needed.
     * \return The flasgs to use.
     */
    int GetEncodeFlags() const override;

 protected:

    /**
     * \brief Get the CA-DEL-001 oid.
     * \return the oid
     */
    const char* GetOid ();

    /**
     * \brief Pack CA-DEL-001 into ASN1C structure.
     * \return pointer to asn1c struct (no data in this case), caller must free.
     */
    void* GetPackedPayload ();

    /**
     * \brief Provides the ASN1C type descriptor for CA-DEL-001
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
    const long kPreliminaryVersion = 8;

    /**
     * \brief Constructs empty signal
     */
    CaDel001Signal (fsm::CCMTransactionId& transaction_id,
                    uint16_t sequence_number);

#ifdef VOC_TESTS
    /**
     * \brief Constructs empty signal with reference to a ccm message
     */
    CaDel001Signal (ccm_Message* ccm,
                    fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                          fsm::Signal::Signal(transaction_id,
                                                                              kCaDel001Signal)
    {
    }
#endif


    /**
     * \brief Provides the prefered encoding for the CA-DEL-001 signal
     * \return prefered fs_Encoding
     */
    fs_Encoding GetPreferedEncoding ();
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_CA_DEL_001_SIGNAL_H_

/** \}    end of addtogroup */
