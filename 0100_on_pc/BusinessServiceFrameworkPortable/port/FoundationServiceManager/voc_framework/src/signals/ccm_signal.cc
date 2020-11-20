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
 *  \file     ccm_signal.cc
 *  \brief    Signal subclass representing a CCM.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/ccm_signal.h"
#include "voc_framework/signals/signal_factory.h"

#include <sstream>
#include <time.h>

// TODO:
// Implement more accessors as needed

namespace fsm
{


CcmSignal::CcmSignal(CCMTransactionId transaction_id) : Signal(transaction_id)
{
        transaction_id.WriteToCString(header_.transaction_id);
}

bool CcmSignal::SetSignallingHeaders(uint64_t time_to_live,
                                     const std::array<unsigned char, CCM_SESSION_ID_LEN>& session_id,
                                     uint16_t message_id,
                                     struct timespec timestamp)
{
    bool return_value = true;

    return_value = return_value ? SetTimeToLive(time_to_live) : return_value;
    return_value = return_value ? SetSessionId(session_id) : return_value;
    return_value = return_value ? SetMessageId(message_id) : return_value;
    return_value = return_value ? SetTimestamp(timestamp) : return_value;

    return return_value;
}

bool CcmSignal::SetTimeToLive(uint64_t time_to_live)
{
    bool return_value = false;

    if (!IsLocked() && time_to_live <= CCM_MAX_NUM_TIME_TO_LIVE_MILLISECONDS)
    {
        header_.time_to_live = time_to_live;

        return_value = true;
    }

    return return_value;
}

bool CcmSignal::SetTimestamp(struct timespec timestamp)
{
    bool return_value = false;

    if (!IsLocked())
    {
        header_.timestamp = timestamp;

        return_value = true;
    }

    return return_value;
}

bool CcmSignal::SetSessionId(std::array<unsigned char, CCM_SESSION_ID_LEN> session_id)
{
    bool return_value = false;

    if (!IsLocked())
    {
        std::memcpy(header_.session_id, session_id.data(), CCM_SESSION_ID_LEN);

        return_value = true;
    }

    return return_value;
}

bool CcmSignal::SetMessageId(uint16_t message_id)
{
    bool return_value = false;

    if (!IsLocked())
    {
        header_.message_id = message_id;

        return_value = true;
    }

    return return_value;
}

bool CcmSignal::SetStoreAndForwardOptions(const ccm_storeAndForwardOptions store_and_forward_options)
{
    bool return_value = false;

    if (!IsLocked())
    {
        header_.delivery_options.store_and_forward = store_and_forward_options;
        return_value = true;
    }

    return return_value;
}

bool CcmSignal::SetRecipients(const std::vector<fsm::UserId>& recipients)
{
    bool return_value = false;

    if (!IsLocked())
    {
        recipients_ = recipients;

        return_value = true;
    }

    return return_value;
}

std::vector<fsm::UserId> CcmSignal::GetRecipients()
{
    return recipients_;
}

bool CcmSignal::GetSessionId(std::array<unsigned char, CCM_SESSION_ID_LEN>& session_id)
{
    bool return_value = false;

    //16 bytes of 0 is undef
    for (int i = 0; i < CCM_SESSION_ID_LEN; i++)
    {
        if (header_.session_id[i] != 0)
        {
            return_value = true;
            break;
        }
    }

    if (return_value)
    {
        std::memcpy(session_id.data(), header_.session_id, CCM_SESSION_ID_LEN);
    }

    return return_value;
}

fsm::UserId CcmSignal::GetSender()
{
    return sender_;
}

bool CcmSignal::GetCertificates(STACK_OF(X509)* certificates)
{
    return certificates_;
}

void CcmSignal::Lock()
{
    lock_ = true;
}

bool CcmSignal::IsLocked()
{
    return lock_;
}

bool CcmSignal::SetPayload(std::shared_ptr<PayloadInterface> payload)
{
    bool return_value = false;

    if (!IsLocked())
    {
        payload_ = payload;

        return_value = true;
    }

    return return_value;
}

std::shared_ptr<PayloadInterface> CcmSignal::GetPayload()
{
    return payload_;
}

std::string CcmSignal::ToString()
{
    PayloadInterface::ExtendedPayloadIdentifier identifier = GetIdentifier();

    std::stringstream string_stream;

    string_stream << "CcmSignal";
    string_stream << ", signal type: " << std::to_string(identifier.signal_type);
    string_stream << ", oid: " << identifier.payload_identifier.oid;

    return string_stream.str();
}

Signal::SignalType CcmSignal::GetSignalType()
{
    Signal::SignalType signal_type = Signal::kTypeUndefined;

    if (payload_)
    {
        signal_type = payload_->GetIdentifier().signal_type;
    }

    return signal_type;
}

PayloadInterface::ExtendedPayloadIdentifier CcmSignal::GetIdentifier() const
{
    PayloadInterface::ExtendedPayloadIdentifier identifier;

    identifier.signal_type = Signal::kTypeUndefined;
    identifier.payload_identifier.oid = std::string();

    if (payload_)
    {
        identifier = payload_->GetIdentifier();
    }

    return identifier;
}

std::vector<PayloadInterface::CodecType> CcmSignal::GetSupportedCodecs() const
{
    return {kCodecTypeCcm};
}

bool CcmSignal::SetPayload(const CodecPayload& codec_payload)
{
    bool return_value = true;

    if (codec_payload.codec_type == kCodecTypeCcm)
    {
        const ccm_Message* ccm = codec_payload.codec_data.ccm_data.ccm;

        header_ = ccm->header;

        sender_ = codec_payload.codec_data.ccm_data.sender;

        certificates_ = codec_payload.codec_data.ccm_data.certificates;

        PayloadIdentifier payload_identifier = PayloadIdentifier();
        payload_identifier.oid = std::string(ccm->payload.content.oid.oid);

        payload_ = SignalFactory::DecodePayload(ccm->payload.content.content->data(),
                                                ccm->payload.content.content->size(),
                                                &payload_identifier,
                                                &(ccm->payload.content.content_encoding),
                                                &(ccm->payload.content.content_version));

        // Note that Transaction id was populated at construction of this signal
        // and the Signal class does not allow changing it.
        // Also note that SignalType is handeld by overloaded GetSignalType().

        // finaly lock the signal against future edits
        lock_ = true;
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

bool CcmSignal::GetEmptyPayload(CodecPayload& codec_payload) const
{
    codec_payload.codec_type = PayloadInterface::kCodecTypeCcm;
    codec_payload.codec_data.ccm_data.recipients = recipients_;

    return true;
}

bool CcmSignal::GetPayload(CodecPayload& codec_payload) const
{
    bool return_value = true;

    codec_payload.codec_data.ccm_data.ccm = new ccm_Message();

    ccm_Message* ccm = codec_payload.codec_data.ccm_data.ccm;

    if (!ccm)
    {
        return_value = false;
    }

    if (return_value && payload_)
    {
        ExtendedPayloadIdentifier payload_identifier = payload_->GetIdentifier();
        fs_Encoding used_encoding = FS_ENCODING_UNDEFINED;
        int encode_flags = 0;
        std::shared_ptr<std::vector<unsigned char>> encoded_payload = nullptr;
        CodecPayload payload_details;
        payload_->GetEmptyPayload(payload_details);

        // if timestamp has not been set by client, set it to current time
        if (return_value && header_.timestamp.tv_sec == 0)
        {
            struct timespec timestamp_not_const = header_.timestamp;
            return_value = timespec_get(&timestamp_not_const, TIME_UTC) != 0;
        }

        // store the oid in header
        if (return_value && !payload_identifier.payload_identifier.oid.empty())
        {
            return_value = ccm_SetOid(&(ccm->payload.content.oid),
                                      payload_identifier.payload_identifier.oid.c_str());
        }

        // encode payload, and then store it in the ccm_Message
        if (return_value)
        {
            encoded_payload = SignalFactory::Encode(payload_, &used_encoding, &encode_flags);

            if (!encoded_payload)
            {
                return_value = false;
            }
        }

        if (return_value)
        {
            ccm->header = header_;
            ccm->payload.content.content = encoded_payload;

            codec_payload.codec_type = kCodecTypeCcm;
            codec_payload.codec_data.ccm_data.recipients = recipients_;
            codec_payload.encode_data.encode_flags = encode_flags;

            if (encode_flags & kEncodeAsPayload)
            {
                // CCM_Payload, content version and encoding goes in envelope
                ccm->payload.content.content_version = payload_details.version;
                ccm->payload.content.content_encoding = used_encoding;
            }
            else
            {
                // not CCM_Paylod, content version and encoding goes in header
                ccm->header.content_version = payload_details.version;
                ccm->header.content_encoding = used_encoding;
            }
        }
    }
    else
    {
        // for now assume that we do not want to produce empty CCMs.
        return_value = false;
    }

    return return_value;
}

} // namespace fsm

/** \}    end of addtogroup */
