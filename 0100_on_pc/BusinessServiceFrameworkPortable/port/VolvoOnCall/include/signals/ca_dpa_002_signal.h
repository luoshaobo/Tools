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
 *  \file     ca_dpa_002_signal.h
 *  \brief    ca_dpa_002 signal
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_CA_DPA_002_SIGNAL_H_
#define VOC_SIGNALS_CA_DPA_002_SIGNAL_H_

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/signal.h"

#include "signals/device_pairing_id.h"
#include "signals/signal_types.h"

// ans1c includes
#include "CA-DPA-002.h"

#include <memory>

namespace volvo_on_call
{
class CaDpa002Signal: public fsm::Signal, public DevicePairingId, public fsm::ResponseInfoData, public fsm::CCMDecodable
#ifdef VOC_TESTS
, public fsm::CCMEncodable
#endif
{

 public:

    /**
     * \brief The oid of this CCM.
     */
    static const std::string oid_;

    ~CaDpa002Signal ();


    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<fsm::Signal> CreateCaDpa002Signal(ccm_Message* ccm,
                                                             fsm::TransactionId& transaction_id);

    /**
     * \brief Reasons why pairing request has been terminated/finished.
     */
    enum FinishedStatus
    {
        kStoppedByCommand,
        kTerminatedRemoteConnectionTimeout,
        kTerminatedPairingTimeout
    };

    /**
     * \brief Checks if pairing request status is Finished. If so, the reason for termination
     *        is provided.
     * \param[out] finished_status reason why request has been terminated/finished. Not populated
     *                             when function returns false
     * \return true when pairing request has been terminated/finished, false otherwise
     */
    bool IsFinished(FinishedStatus& finished_status);

    /**
     * \brief Checks if pairing request status is Initiated.
     * \return true when pairing request has been initiated, false otherwise
     */
    bool IsInitiated();

    /**
     * \brief Checks if remote connection session id has been provided. If the session has
     *        been provided in the signal then it is returned
     * \param[out] session_id remote connection session id. Not populated when function returns false
     * \return True if remote connection session id has been provided, false otherwise.
     */
    bool GetRemoteConnectionSessionId(std::string& session_id);

    // ***
    // Signal implementation start.
    // ***

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "CaDpa002Signal"; }

 private:

    static const long kSignalFlowVersion = 1;
    static const long kSchemaVersion = 1;
    static const long kPreliminaryVersion = 8;

    /**
     * \brief Tracks if the signal is valid.
     */
    bool valid_ = false;

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    CaDpa002Signal(ccm_Message* ccm,
                   fsm::TransactionId& transaction_id);

    /**
     * \brief unpacks a asn1c CA_DPA_002_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_ca_dpa_002 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(CA_DPA_002_t* asn1c_ca_dpa_002);

    // ***
    // Signal implementation end.
    // ***

    /**
     * \brief Status of the request to initiate pairing
     */
    enum Status
    {
        kStatusNothing,
        kStatusInitiated,
        kStatusFinished
    };

    /**
     * \brief Status of the request to initiate pairing.
     */
    Status status_ = kStatusNothing;

    /**
     * \brief Reason why pairing request has failed.
     */
    FinishedStatus status_finished_;

    /**
     * \brief tracks if remote connection session id is set
     */
    bool session_id_set_ = false;

    /**
     * \brief remote connection session id
     */
    std::string session_id_;


#ifdef VOC_TESTS

    // During VOC unit test we want to run both encode and decode
    // as this signal is only expected to be decoded inproduction
    // the encode support is under ifdef.

 public:
    /**
     * \brief Constructs empty signal
     */
    CaDpa002Signal(fsm::CCMTransactionId& transaction_id,
                   uint16_t message_id);

    /**
     * \brief Sets status to initiated
     */
    void SetInitiated();

    /**
     * \brief Sets status to finished and assigns corresponding finished status
     * \param[in] status status to set
     */
    void SetFinished(FinishedStatus status);


    /**
     * \brief Sets remote connection session id
     * \param[in] session_id session id
     */
    void SetRemoteConnectionSessionId(std::string session_id);

    /**
     * \brief Gets the oid of the payload content.
     * \return the oid.
     */

    // ***
    // CCMEncodable virtual functions implementation start.
    // ***

    const char* GetOid() {return oid_.c_str();};

 protected:

    /**
     * \brief Provide the ContentVersion implemented for the CCM payload
     * \return The content version
     */
    fs_VersionInfo GetContentVersion()
    {
            fs_VersionInfo version_info;
            version_info.signal_flow_version = kSignalFlowVersion;
            version_info.schema_version = kSchemaVersion;
            version_info.preliminary_version = kPreliminaryVersion;
            return version_info;
    };

    /**
     * \brief Pack CCM payload into ASN1C structure, used while encoding payload
     * \return pointer to asn1c struct containing payload, caller must free.
     */
    void* GetPackedPayload();

    /**
     * \brief Provides the ASN1C type descriptor for the CCM payload
     * \return pointer to ASN1C type decriptor, caller must not free
     */
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor() {return &asn_DEF_CA_DPA_002;};

    // ***
    // CCMEncodable virtual functions implementation stop.
    // ***

#endif

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_CA_DPA_002_SIGNAL_H_

/** \}    end of addtogroup */
