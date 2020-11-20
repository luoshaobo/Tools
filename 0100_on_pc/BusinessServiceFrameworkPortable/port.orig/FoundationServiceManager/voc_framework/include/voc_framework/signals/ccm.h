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
 *  \file     ccm.h
 *  \brief    ccm util class
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CCM_H_
#define VOC_FRAMEWORK_SIGNALS_CCM_H_

//asn1c includes
#include "asn_application.h"
#include "ResponseInfo.h"

#include <openssl/x509.h>

#include "fsm_ccm.h"

#include "voc_framework/transactions/transaction_id.h"
#include "voc_framework/signals/payload_interface.h"

namespace fsm
{

class CCM : public PayloadInterface
{

 public:

    /**
     * Creates an empty ccm wrapper.
     */
    CCM ();

    /**
     * \brief Creates a ccm wrapper with all necessery header fields for
     * for a valid encodable CCM.
     * Note: CCM version is set by FSM CCM lib, timestamp is set
     *       during encode, thus both are omitted here.
     *
     * \param[in] transaction_id  value to put in the CCM header transactionId field
     * \param[in] message_id      value to put in the CCM header messageId field
     */
    CCM (CCMTransactionId& transaction_id,
         uint16_t message_id);

    //Forbid to use copy constructor
    CCM( const CCM &obj) = delete;
    CCM & operator = (const CCM &ccmsg) = delete;

    /**
     * Creates a wrapper around an existing message.
     * Assumes ownership of the pointer.
     */
    CCM (ccm_Message* ccm);

    virtual ~CCM ();

    /**
     * \brief Set the timeToLive field in CCM header
     *
     * \param[in] time_to_live value to put in the CCM header timeToLive field
     *
     * \return nothing
     */
    void SetTimeToLive (uint64_t time_to_live);

    /**
     * \brief Set the transactionId field in CCM header.
     *
     * \param[in] transaction_id value to put in the CCM header transactionId field
     *
     * \return nothing
     */
    void SetTransactionId (CCMTransactionId& transaction_id);

    /**
     * \brief Set the messageId field in CCM header.
     *
     * \param[in] message_id value to put in the CCM header messageId field
     *
     * \return nothing
     */
    void SetMessageId (uint16_t message_id);

    /**
     * \brief Set the certificates contained in this CCM. Can only be called once
     *        in order to, sort of, protect the immutability of a signal.
     *        Note that this gives ownership of the certificates to the CCM
     *        caller must not free them.
     *
     * \param[in] certificates The stack of certificates to set. Nullptr is
     *                         interpreted as setting that there are no certs.
     *
     * \return False if already called, otherwise true.
     */
    bool SetCertificates (STACK_OF(X509)* certificates);

    /**
     * \brief Returns a PayloadIdentifier for this payload.
     *
     * \return A PaylaodIdentifier.
     */
    ExtendedPayloadIdentifier GetIdentifier() const;

    /**
     * \brief Returns a list of codecs supported by this payload.
     *
     * \return A list of codecs supported by this payload.
     */
    std::vector<CodecType> GetSupportedCodecs() const;

    /**
     * \brief Not supported, use legacy encode + factory functions.
     *        Legacy decode has a special path in signal factory
     *        as legacy Signals are not default constructable.
     *
     * \param[in] codec_payload ignored
     *
     * \return False.
     */
    bool SetPayload(const CodecPayload& codec_payload);

    /**
     * \brief Gets this payloads contents in format consumable by ccm_codec.
     *
     * \param[out] codec_payload  Will be populated with payload contents.
     *
     * \return False on failure, true on success.
     */
    bool GetPayload(CodecPayload& codec_payload) const;

    /**
     * \brief Intialize a CodecPayload for this payload so that
     *        is is ready to be populated by a codec and passed to
     *        SetPayload.
     *
     * \param[in/out] codec_payload CodecPayload to intialize.
     *
     * \return False on failure, otherwise true.
     */
    bool GetEmptyPayload(CodecPayload& codec_payload) const;

 protected:

    std::shared_ptr<ccm_Message> message = nullptr;

    /**
     * \brief certificates attached to this CCM from CMS structure.
     */
    STACK_OF(X509)* certificates_ = nullptr;

    /**
     * \brief tracks whether attached certificates have been set.
     */
    bool certificates_set_ = false;
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CCM_H_

/** \}    end of addtogroup */
