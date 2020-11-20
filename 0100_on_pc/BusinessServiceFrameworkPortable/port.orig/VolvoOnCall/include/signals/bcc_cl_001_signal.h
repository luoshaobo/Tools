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
 *  \file     bcc_cl_001_signal.h
 *  \brief    bcc_cl_001 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_BCC_CL_001_SIGNAL_H_
#define VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

// fsm includes
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/signal.h"
#include "signals/signal_types.h"

// ans1c includes
#include "BCC-CL-001.h"

#include <memory>

namespace volvo_on_call
{

class BccCl001Signal: public fsm::Signal, public fsm::CCMDecodable
{

 public:

    static const std::string oid;

    /**
    * \brief Types of adverts that can be present in BCC CL 001
    */
     enum  Advert {None, Honk, Flash, HonkAndFlash};

    ~BccCl001Signal ();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<fsm::Signal> CreateBccCl001Signal(ccm_Message* ccm,
                                                             fsm::TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "BccCl001Signal"; }


    /**
    * \brief Returns information about the requested advert type.
    * \return Returns the type of requested advert. Advert::None means
    *         that no advert is requested
    */
    Advert GetSupportedAdvert() {return supportedAdvert;}


 protected:

    Advert supportedAdvert = None;

    /**
     * \brief Provide the bcc_cl_001 oid.
     * \return the oid
     */
    const char* GetOid ();

    /**
     * \brief Pack BCC-CL-001 into ASN1C structure.
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload ();

    /**
     * \brief Provides the ASN1C type descriptor for BCC-CL-001
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

    bool valid = false;

    /**
     * \brief Constructs empty signal
     */
    BccCl001Signal (  fsm::CCMTransactionId& transaction_id,
                      uint16_t sequence_number);

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    BccCl001Signal (ccm_Message* ccm,
                    fsm::TransactionId& transaction_id);


  private:
    /**
     * \brief unpacks a asn1c BCC_CL_001_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_bcc_cl_001 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(BCC_CL_001_t* asn1c_bcc_cl_001);
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_BCC_CL_001_SIGNAL_H_

/** \}    end of addtogroup */
