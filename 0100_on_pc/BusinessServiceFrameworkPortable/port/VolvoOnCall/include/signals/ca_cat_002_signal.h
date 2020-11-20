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
 *  file     ca_cat_002.h
 *  brief    CA_CAT_002 signal
 *  author   Maksym Mozok
 *
 *  addtogroup VolvoOnCall
 *  {
 */

// fsm includes
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/signal.h"

//voc includes
#include "signals/signal_types.h"

// ans1c includes
#include "CA-CAT-002.h"


#ifndef CA_CAT_002_SIGNAL_H
#define CA_CAT_002_SIGNAL_H

namespace volvo_on_call
{

class CaCat002Signal: public fsm::Signal, public fsm::CCMDecodable, public fsm::ResponseInfoData
{

  public:

    static const std::string oid_;

    /**
    * \brief Default destructor for CA_CAT_002
    */
    ~CaCat002Signal();

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<fsm::Signal> CreateCaCat002Signal(ccm_Message* ccm,
                                                             fsm::TransactionId& transaction_id);

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "CaCat002Signal"; }

#ifdef VOC_TESTS
    /**
     * \brief Constructs empty signal
     */

    CaCat002Signal(fsm::CCMTransactionId& transaction_id,
                   uint16_t sequence_number) : fsm::CCM::CCM(),
                                               fsm::Signal::Signal(transaction_id,
                                                                   kCaCat002Signal)
    {
    };
#endif

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    CaCat002Signal (ccm_Message* ccm,
                    fsm::TransactionId& transaction_id);

 protected:

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion ();

    const long kSignalFlowVersion = 1;
    const long kSchemaVersion = 1;
    const long kPreliminaryVersion = 8;

 private:

    bool valid = false;

    /**
     * \brief unpacks a asn1c CA_CAT_002_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_ca_cat_002 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(CA_CAT_002_t* asn1c_ca_cat_002);
};

} // namespace volvo_on_call

#endif // CA_CAT_002_SIGNAL_H
