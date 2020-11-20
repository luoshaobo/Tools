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
 *  \file     ca_dpa_021_signal.h
 *  \brief    ca_dpa_021 signal
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_CA_DPA_021_SIGNAL_H_
#define VOC_SIGNALS_CA_DPA_021_SIGNAL_H_

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/response_info_data.h"
#include "voc_framework/signals/signal.h"

#include "signals/device_pairing_id.h"
#include "signals/voc_frame_codec.h"

// ans1c includes
#include "CA-DPA-021.h"

#include <memory>

namespace volvo_on_call
{
class CaDpa021Signal: public fsm::Signal, public DevicePairingId, public fsm::ResponseInfoData, public fsm::CCMEncodable
#ifdef VOC_TESTS
, public fsm::CCMDecodable
#endif
{

 public:

    /**
     * \brief The oid of this CCM.
     */
    static const std::string oid_;

    ~CaDpa021Signal ();

    /**
     * \brief Constructs empty signal
     */
    CaDpa021Signal(fsm::CCMTransactionId& transaction_id,
                   uint16_t message_id);

    /**
     * \brief Set the frame in the signal.
     *
     * \param[in] codec   VocFrameCodec to use when encoding the frame.
     *                    Client is expected to have prepared it with
     *                    keys as needed.
     * \param[in] frame   Frame to set.
     *
     * \return True on success, false on failure.
     */
    bool SetFrame(VocFrameCodec codec, const VocFrameEncodableInterface& frame);

    // ***
    // Signal implementation start.
    // ***

    /**
     * \brief String identifier for printouts
     */
    std::string ToString() {return "CaDpa021Signal"; }

    // ***
    // Signal implementation end.
    // ***


    // ***
    // CCMEncodable virtual functions implementation start.
    // ***

    /**
     * \brief Gets the oid of the payload content.
     * \return the oid.
     */
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
    asn_TYPE_descriptor_t* GetPayloadTypeDescriptor() {return &asn_DEF_CA_DPA_021;};

    // ***
    // CCMEncodable virtual functions implementation stop.
    // ***

 private:

    static const long kSignalFlowVersion = 1;
    static const long kSchemaVersion = 1;
    static const long kPreliminaryVersion = 8;

    /**
     * \brief Tracks if the signal is valid.
     */
    bool valid_ = false;

    /**
     * \brief The vod frame.
     */
    std::vector<unsigned char> frame_;

#ifdef VOC_TESTS

    // During VOC unit test we want to run both encode and decode.
    // As this signal is only expected to be encoded in production
    // the decode support is under ifdef.

 public:

    /**
     * \brief Constructs signal from ccm message
     * \param[in] ccm            message to create the signal from
     * \param[in] transaction_id ccm header transactionId
     * \return shared pointer to created signal, or empty shared pointer
     *         if supplied message was invalid.
     */
    static std::shared_ptr<CaDpa021Signal> CreateCaDpa021Signal(ccm_Message* ccm,
                                                                fsm::TransactionId& transaction_id);
    /**
     * \brief Get the contained Voc Frame.
     *
     * \parame[in] codec VocFrameCodec to use when decoding the frame.
     *                   Caller is expected to have prepared the codec
     *                   with keys as needed.
     *
     * \return Pointer to the contained frame,
     *         or nullptr if this signal is in bad state.
     */
    std::shared_ptr<fsm::Signal> GetVocFrame(VocFrameCodec& codec);

 private:

    /**
     * \brief Contructs a signal populated from a ccm_Message
     */
    CaDpa021Signal(ccm_Message* ccm,
                   fsm::TransactionId& transaction_id);

    /**
     * \brief unpacks a asn1c CA_DPA_021_t struct and populates this
     *        message with its contents
     * \param[in] asn1c_ca_dpa_021 the struct to unpack
     * \return true if successful, false otherwise
     */
    bool UnpackPayload(CA_DPA_021_t* asn1c_ca_dpa_021);



#endif

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_CA_DPA_021_SIGNAL_H_

/** \}    end of addtogroup */
