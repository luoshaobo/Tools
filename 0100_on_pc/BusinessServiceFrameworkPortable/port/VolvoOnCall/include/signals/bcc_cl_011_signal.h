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
 *  \file     bcc_cl_011_signal.h
 *  \brief    BCC_CL_011 signal
 *  \author   Iulian Sirghi
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_BCC_CL_011_SIGNAL_H_
#define VOC_SIGNALS_BCC_CL_011_SIGNAL_H_


// fsm includes
#include "voc_framework/signals/location_data.h"
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/signal.h"

// voc includes
#include "signals/signal_types.h"

// ans1c includes
#include "BCC-CL-011.h"

#include <memory>
#include <vector>

namespace volvo_on_call
{

class BccCl011Signal: public fsm::Signal, public fsm::CCMEncodable, public fsm::LocationData, public fsm::ResponseInfoData
{

public:
   static const std::string oid;

   ~BccCl011Signal();

   /**
    * \brief Constructs signal from supplied data
    * \param[in] transaction_id  ccm header transactionId
    * \param[in] sequence_number ccm header sequenceNumber
    * \return shared pointer to created signal, or empty shared pointer
    *         if supplied data was invalid.
    */
   static std::shared_ptr<BccCl011Signal> CreateBccCl011Signal(fsm::CCMTransactionId& transaction_id,
                                                               uint16_t sequence_number);

protected:

#ifdef VOC_TESTS
    /**
     * \brief Constructs a bcc_cl_011 from a binary ccm_Message payload.
     *        This is only here to enable unittest to subclass this
     *        fsm::Signal and create a decodable version.
     * \param[in] ccm The message to construct from.
     * \param[in] transaction_id The transaction id to constrcut the message from.
     */
    BccCl011Signal (ccm_Message* ccm,
                    fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                          fsm::Signal::Signal(transaction_id,
                                                                              kBccCl011Signal) {}
#endif

   /**
    * \brief Provide the BCC_FUNC_001 oid.
    * \return the oid
    */
   const char* GetOid ();

   /**
    * \brief Pack BCC-FUNC-001 into ASN1C structure.
    * \return pointer to asn1c struct containing payload, caller must free.
    */
   void* GetPackedPayload ();

   /**
    * \brief Provides the ASN1C type descriptor for BCC-FUNC-001
    * \return pointer to ASN1C type decriptor, caller must not free
    */
   asn_TYPE_descriptor_t* GetPayloadTypeDescriptor ();

   /**
    * \brief Provide the ContentVersion implemented for the CCM payload
    * \return The content version
    */
   fs_VersionInfo GetContentVersion ();

   /**
    * \brief String identifier for printouts
    */
   std::string ToString() { return "BccCl011Signal"; }


private:

   const long kSignalFlowVersion = 1;
   const long kSchemaVersion = 1;
   const long kPreliminaryVersion = 10;

   bool valid = false;

   /**
    * \brief Constructs empty signal, must be populated via setters
    */
   BccCl011Signal (fsm::CCMTransactionId& transaction_id,
                   uint16_t sequence_number);
};

} //namespace

#endif //VOC_SIGNALS_BCC_CL_011_SIGNAL_H_

/** \}    end of addtogroup */
