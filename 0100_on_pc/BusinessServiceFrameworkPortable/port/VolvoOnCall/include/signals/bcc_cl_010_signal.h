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
 *  \file     bcc_cl_010_signal.h
 *  \brief    BCC_CL_010 signal
 *  \author   Iulian Sirghi
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_BCC_CL_010_SIGNAL_H_
#define VOC_SIGNALS_BCC_CL_010_SIGNAL_H_

// fsm includes
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/signal.h"

// voc includes
#include "signals/signal_types.h"


// ans1c includes
#include "BCC-CL-010.h"

#include <memory>
#include <vector>


namespace volvo_on_call
{

class BccCl010Signal : public fsm::Signal, public fsm::CCMDecodable
{
public:
    static const std::string oid;

    ~BccCl010Signal();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<fsm::Signal> CreateBccCl010Signal(ccm_Message* ccm,
                                                             fsm::TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() { return "BccCl010Signal"; }

 protected:
#ifdef VOC_TESTS
    /**
     * \brief Constructs empty signal, must be populated via setters
     *        This is only here to enable unittest to subclass this
     *        fsm::Signal and create a encodable version.
     */
    BccCl010Signal(fsm::CCMTransactionId& transaction_id,
                   uint16_t sequence_number) : fsm::CCM::CCM(transaction_id, sequence_number),
                                               fsm::Signal::Signal(transaction_id, kBccCl010Signal) {}
#endif
    /**
     * \brief Constructs a bcc_cl_010 from a binary ccm_Message payload.
     * \param[in] ccm The message to construct from.
     * \param[in] transaction_id The transaction id to constrcut the message from.
     */
    BccCl010Signal(ccm_Message* ccm,
                   fsm::TransactionId& transaction_id);



    /**
     * \brief Provide the bcc_cl_010 oid.
     * \return the oid
     */
    const char* GetOid();

    /**
     * \brief Provides the ASN1C type descriptor for BCC-CL-010
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor();

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 10;

    bool valid = false;

private:
  /**
   * \brief unpacks a asn1c BCC_CL_010_t struct and populates this
   *        message with its contents
   * \param[in] asn1c_bcc_cl_010 the struct to unpack
   * \return true if successful, false otherwise
   */
  bool UnpackPayload(BCC_CL_010_t* asn1c_bcc_cl_010);

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_010_SIGNAL_H_

/** \}    end of addtogroup */
