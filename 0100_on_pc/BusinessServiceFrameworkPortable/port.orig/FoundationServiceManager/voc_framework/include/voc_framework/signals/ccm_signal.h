/**
 * Copyright (C) 2016, 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     ccm_signal.h
 *  \brief    Signal subclass representing a CCM.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CCM_SIGNAL_H_
#define VOC_FRAMEWORK_SIGNALS_CCM_SIGNAL_H_

#include <array>
#include <memory>
#include <vector>

//asn1c
#include "asn_application.h"

//marben
#include "aipuptyp.h"

//UserManager
#include "usermanager_types.h"

//ccm
#include "fsm_ccm.h"

//voc_framework
#include "voc_framework/signals/payload_interface.h"
#include "voc_framework/signals/signal.h"


namespace fsm
{

/**
 * \brief CcmSignal holds a CCM. It provides functions for setting and retrieving
 *        the header data, and holds a PayloadInterface payload.
 *        Some CCM header fields have no setter, this is beacause they are set
 *        automatically during encode.
 *        Becasue incomming CcmSignals may be shared by several transactions
 *        they can be locked to protect their data. CcmSignal will lock itself
 *        if populated via PaylaodInterface, its alos possible for a client
 *        to explicitly lock a signal it has created.
 *        CcmSignal is a realization of PayloadInterface, and if populated using
 *        SetPayload it will be immutable, any further calls to setters will fail.
 */
// CcmSignal serves mostly as a wrapper for fsm_ccm and provides the conversion
// from voc_fraemwork c++ world to fsm_ccm c-style world.
// One might consider putting the functionality of fsm_ccm completely in CcmCodec
// using CcmSignal as the data holder.
class CcmSignal : public Signal, public PayloadInterface
{

 public:

    /**
     * \brief Constructs an empty Signal.
     *
     * \param[in] transaction_id Optional transaction id, if not
     *                           provided a transaction id will
     *                           be generated-
     */
    CcmSignal(CCMTransactionId transaction_id = CCMTransactionId());

    /**
     * \brief Set all header fields required by signalling service.
     *        Note that transaction id is provided at constructor as all Signals
     *        msut have a transaction id.
     *
     * \param[in] time_to_live TTL of the CCM.
     * \param[in] session_id   Session id of the CCM.
     * \param[in] message_id   Message id of the CCM.
     * \param[in] timestamp    Optional timestamp of the CCM, if not set will be automatically
     *                         set during encode.
     *
     * \return False if locked.
     */
    bool SetSignallingHeaders(uint64_t time_to_live,
                              const std::array<unsigned char, CCM_SESSION_ID_LEN>& session_id,
                              uint16_t message_id,
                              struct timespec timestamp = {0, -1});

    /**
     * \brief Set theCCM header ttl.
     *
     * \param[in] time_to_live The ttl to set. MAy not exceed CCM_MAX_NUM_TIME_TO_LIVE_MILLISECONDS.
     *
     * \return False if locked or ttl to large.
     */
    bool SetTimeToLive(uint64_t time_to_live);

    /**
     * \brief Set the CCM header timestamp. If not set
     *        the current time when the signal is encoded
     *        will be used.
     * \param[in] Timestamp timestamp to set.
     *
     * \return False if locked.
     */
    bool SetTimestamp(struct timespec timestamp);

    /**
     * \brief Set the session id .
     *
     * \param[in] session_id The seesion id to set.
     *
     * \return False if locked.
     */
    bool SetSessionId(std::array<unsigned char, CCM_SESSION_ID_LEN> session_id);

    /**
     * \brief Set the message id of this CCM.
     *
     * \param[in] message_id The message id to set.
     *
     * \return False if locked.
     */
    bool SetMessageId(uint16_t message_id);

    /**
     * \brief Set the store and forward option.
     *        This option is used by cloud to decide
     *        how to deliver the message.
     *
     * \param[in] store_and_forward_options store and forward option
     *                                      to be used by cloud
     *
     * \return False if locked.
     */
    bool SetStoreAndForwardOptions(const ccm_storeAndForwardOptions store_and_forward_options);

    /**
     * \brief Set the recipients of this CCM.
     *
     * \param[in] recipients List of users CCM is for.
     *
     * \return False if locked.
     */
    bool SetRecipients(const std::vector<fsm::UserId>& recipients);

    /**
     * \brief Get the recipients of this CCM.
     *
     * \return Recipients list of this CCM.
     */
    std::vector<fsm::UserId> GetRecipients();

    /**
     * \brief Get the session id.
     *
     * \param[out] session_id The session id willl be appended if set.
     *
     * \return True if the session id is set, otherwise false.
     */
    bool GetSessionId(std::array<unsigned char, CCM_SESSION_ID_LEN>& session_id);

    /**
     * \brief Get the sender of the message.
     *
     * \return The send of the message, or kUndefinedUserId if
     *         if sender is not known.
     */
    fsm::UserId GetSender();

    /**
     * \brief Get certificates attached in CCM (CMS), if any.
     *
     * \param[out] certificates Certificates attached to the CCM will be added to
     *                          this stack if any.
     *                          Caller must free these using X509_free,
     *                          sk_X509_pop_free or simillar once done with them.
     *
     * \return True if certificates available, false if not.
     */
    bool GetCertificates(STACK_OF(X509)* certificates);

    /**
     * \brief Lock this CcmSignal.
     */
    void Lock();

    /**
     * \brief Check if this signal is locked.
     *        A CcmSignal will be locked if it has been populated
     *        via PayloadInterface or explicitly by the client.
     *
     * \return True if locked.
     */
    bool IsLocked();

    /**
     * \brief Set the payload to be contained in the CCM.
     *
     * \param[in] payload The payload to set, or nullptr to clear.
     *
     * \return False if locked.
     */
    bool SetPayload(std::shared_ptr<PayloadInterface> payload);

    /**
     * \brief Get the payload contained in the CCM.
     *
     * \return The payload, or null if none.
     */
    std::shared_ptr<PayloadInterface> GetPayload();


    // ***
    // Signal function overrides start.
    // ***

    /**
     * \copydoc Signal::ToString
     */
    std::string ToString();

    /**
     * \copydoc Signal::GetSignalType
     */
    SignalType GetSignalType();

    // ***
    // Signal function overrides end.
    // ***

    // ***
    // PayloadInterface realization start.
    // ***

    /**
     * \brief Returns the contained payloads identifier,
     *        or a empty identifier if there is no payload.
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
     * \brief Populate this payload with data from codec.
     *
     * \param[in] codec_payload Data to populate from.
     *
     * \return False on failure, true on success.
     */
    bool SetPayload(const CodecPayload& codec_payload);

    /**
     * \brief Gets this payloads contents in format consumable by codec.
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

    // ***
    // PayloadInterface realization end.
    // ***

 private:

    /**
     * \brief Holds the ccm header data.
     */
    ccm_Header header_ = ccm_Header();

    /**
     * \brief List of recipients for this signal.
     */
    std::vector<fsm::UserId> recipients_;

    /**
     * \brief Sender of this signal.
     */
    fsm::UserId sender_ = kUndefinedUserId;

    /**
     * \brief certificates attached to the CCM CMS.
     */
    STACK_OF(X509)* certificates_ = nullptr;

    /**
     * \brief Tracks whether this ccm signal is locked, if it is no
     *        modifications are allowed.
     */
    bool lock_ = false;

    /**
     * \brief The payload contained in the CCM.
     */
    std::shared_ptr<PayloadInterface> payload_ = nullptr;
};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CCM_SIGNAL_H_

/** \}    end of addtogroup */
