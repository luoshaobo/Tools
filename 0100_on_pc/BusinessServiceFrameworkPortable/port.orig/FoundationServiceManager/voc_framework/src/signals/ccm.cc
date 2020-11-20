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
 *  \file     ccm.cc
 *  \brief    Connectivity Compact Messages util class
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "keystore.h"
#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_codec.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm_encodable.h"

#include <cstring>
#include <memory>
#include <vector>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

/**
 * \brief Utility implementation of ASN1C write encoded bytes to buffer callback.
 *
 * \param[in] buffer     Encoded bytes to write.
 * \param[in] size       Number of bytes to write.
 * \param[in] clientdata Should be a valid FSData* which will be filled with the encode bytes.
 * \return 0 on success, -1 on failure
 */
static int voc_EncodeASN1CWriteCB(const void *buffer, size_t size, void *clientdata)
{
    fs_Data* encode_buffer = (fs_Data*)clientdata;

    if (!encode_buffer)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Bad buffer while writing encoded asn1 bytes.");
        return -1;
    }

    if (size == 0)
    {
        encode_buffer->data = NULL; // modify klocwork warning,yangjun add 20181107
        // do nothing
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Asked to consume 0 encoded asn1 bytes.");
        return 0;
    }
    //TODO: consider a more intelligent realloc strategy
    void* result = realloc(encode_buffer->data, (encode_buffer->num_bytes) + size);

    if (!result)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "failed to reallocate buffer while writing"
                                               "encoded asn1c bytes.");
        return -1;
    }
    encode_buffer->data = result;

    char* start = ((char*) encode_buffer->data) + encode_buffer->num_bytes;
    memcpy(start, buffer, size);

    encode_buffer->num_bytes = encode_buffer->num_bytes + size;

    return 0;
}

CCM::CCM ()
{
    message = std::make_shared<ccm_Message>();
}

CCM::CCM (CCMTransactionId& transaction_id,
          uint16_t message_id)
{
    message = std::make_shared<ccm_Message>();

    transaction_id.WriteToCString(message->header.transaction_id);
    message->header.message_id = message_id;

}

CCM::CCM (ccm_Message* ccm)
{
    message = std::shared_ptr<ccm_Message>(ccm);
}

CCM::~CCM ()
{
    if (certificates_)
    {
        sk_X509_pop_free(certificates_, X509_free);
    }
}
//CCM & CCM::operator = (const CCM &ccmsg)

void CCM::SetTimeToLive(uint64_t time_to_live)
{
    message->header.time_to_live = time_to_live;
}

void CCM::SetTransactionId(CCMTransactionId& transaction_id)
{
    transaction_id.WriteToCString(message->header.transaction_id);
}

void CCM::SetMessageId(uint16_t message_id)
{
    message->header.message_id = message_id;
}

bool CCM::SetCertificates(STACK_OF(X509)* certificates)
{
    bool return_value = false;

    if (!certificates_set_)
    {
        certificates_set_ = true;

        if (certificates)
        {
            certificates_ = certificates;
        }

        return_value = true;
    }

    return return_value;
}

void CCMEncodable::SetSessionId(std::string session_id)
{
    if (message->header.session_id && session_id.c_str() && (session_id.size() >= CCM_SESSION_ID_LEN))
    {
        std::memcpy(message->header.session_id, session_id.data(), CCM_SESSION_ID_LEN);
    }
}

void CCMEncodable::SetStoreAndForwardOptions(const ccm_storeAndForwardOptions store_and_forward_options)
{
     message->header.delivery_options.store_and_forward = store_and_forward_options;
}

bool CCMDecodable::GetSessionId(std::string& session_id)
{
    bool session_id_set = false;

    //16 bytes of 0 is undef
    for (int i = 0; i < CCM_SESSION_ID_LEN; i++)
    {
        if (message->header.session_id[i] != 0)
        {
            session_id_set = true;
            break;
        }
    }

    if (session_id_set)
    {
        session_id.append(message->header.session_id, CCM_SESSION_ID_LEN);
    }

    return session_id_set;
}

void CCMEncodable::SetRecipients(const std::vector<fsm::UserId>& recipients)
{
    recipients_ = recipients;
}

std::vector<fsm::UserId> CCMEncodable::GetRecipients()
{
    return recipients_;
}

bool CCMDecodable::GetCertificates(STACK_OF(X509)* certificates)
{
    bool return_value = certificates_set_ && certificates && certificates_;

    if (return_value)
    {
        int num_certs = sk_X509_num(certificates_);

        for (int i = 0; i < num_certs; i++)
        {
            X509* certificate = sk_X509_value(certificates_, i);

            X509_up_ref(certificate);

            sk_X509_push(certificates, certificate);
        }
    }

    return return_value;
}

std::shared_ptr<std::vector<unsigned char>> CCMEncodable::Encode()
{
    bool return_value = false;

    std::shared_ptr<std::vector<unsigned char>> return_buffer = nullptr;

    fs_Data encoded_data = {0};

    //set the timestamp
    return_value = timespec_get(&message->header.timestamp, TIME_UTC) != 0;

    if (return_value)
    {
        return_value = ccm_SetOid(&(message->payload.content.oid), GetOid());
    }

    if (return_value)
    {
        return_value = EncodePayload(*message);
    }

    if (return_value)
    {
        return_value = ccm_Encode(&encoded_data,
                                  message.get(),
                                  recipients_,
                                  CcmCodec::GetCcmEncodeFlags(GetEncodeFlags()));
    }

    if (return_value)
    {
        //TODO: get rid of this memcopy
        return_buffer = std::make_shared<std::vector<unsigned char>>();
        return_buffer->assign((reinterpret_cast<unsigned char*>(encoded_data.data)),
                            ((reinterpret_cast<unsigned char*>(encoded_data.data))+ encoded_data.num_bytes));
    }

    if(encoded_data.data)
    {
        free(encoded_data.data);
    }

    return return_buffer;
}

bool CCMDecodable::DecodePayload (void** asn1c_packed_payload,
                         asn_TYPE_descriptor_t* type_descriptor,
                         fs_VersionInfo accepted_version)
{
    // check input
    if ((!asn1c_packed_payload) || (!type_descriptor))
    {
        return false;
    }

    // Check that message content version info matches what we implement.
    // Note that this relies on fsm ccm lib being helpful and copying header.content_version
    // into payload.content.content_version in the case of non CCM-Payload.
    //TODO: needs more thought, backwards compatability etc...
    if (!ccm_CompareVersionInfo(&accepted_version, &(message->payload.content.content_version)))
    {
        return false;
    }

    // decode
    asn_dec_rval_t result = {};

    // Note that this relies on fsm ccm lib being helpful and copying header.content_encoding
    // into payload.content.content_encoding in the case of non CCM-Payload.
    switch (message->payload.content.content_encoding)
    {
    case FS_ENCODING_DER :
        result = ber_decode(0,
                            type_descriptor,
                            asn1c_packed_payload,
                            message->payload.content.content->data(),
                            message->payload.content.content->size());
        break;
    case FS_ENCODING_UPER :
        result = uper_decode_complete(0,
                                      type_descriptor,
                                      asn1c_packed_payload,
                                      message->payload.content.content->data(),
                                      message->payload.content.content->size());
        break;
    case FS_ENCODING_PER :
        //TODO: implement, but not supported by ASN1C, wait for marben?
    case FS_ENCODING_BER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_CER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_XER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_XML :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_OER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_UNDEFINED :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Found unimplemented content encoding.");
        return false;
    default :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Found invalid content encoding.");
        return false;
    }

    if (result.code != RC_OK)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode payload");
        return false;
    }
    return true;
}

bool CCMEncodable::EncodePayload (ccm_Message& ccm)
{
    void* asn1c_packed_payload = GetPackedPayload();
    asn_TYPE_descriptor_t* type_descriptor = GetPayloadTypeDescriptor();
    fs_Encoding prefered_encoding = GetPreferedEncoding();

    if ((!asn1c_packed_payload) || (!type_descriptor))
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to retrieve packed payload.");
        return false;
    }

    // buffer to hold the payload data during encode
    // TODO replace with vector
    fs_Data encode_buffer;
    encode_buffer.data = NULL;
    encode_buffer.num_bytes = 0;

    asn_enc_rval_t result = {};
    fs_Encoding used_encoding = FS_ENCODING_UNDEFINED;

    switch (prefered_encoding)
    {
    case FS_ENCODING_DER :
        result = der_encode(type_descriptor,
                            asn1c_packed_payload,
                            &voc_EncodeASN1CWriteCB,
                            &encode_buffer);
        used_encoding = FS_ENCODING_DER;
        break;
    case FS_ENCODING_UPER :
        result = uper_encode(type_descriptor,
                             asn1c_packed_payload,
                             &voc_EncodeASN1CWriteCB,
                             &encode_buffer);
        used_encoding = FS_ENCODING_UPER;
        break;
    case FS_ENCODING_BER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_CER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_XER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_XML :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_OER :
        //TODO: implement?
        //      from reading foundation services ASN1 spec it seems
        //      only DER, PER, UPER are supported
    case FS_ENCODING_PER :
        //TODO: implement, but not supported by ASN1C, wait for marben?
    case FS_ENCODING_UNDEFINED :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Received unimplemented prefered encoding.");
        return false;
    default :
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Received invalid prefered encoding.");

        return false;
    }

    if (result.encoded <= 0)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to encode payload.");
        if (encode_buffer.data)
        {
            free(encode_buffer.data);
        }
        encode_buffer.num_bytes = 0;
        return false;
    }
    encode_buffer.num_bytes = result.encoded;


    // store encoded payload in message
    if (GetEncodeFlags() & kEncodeAsPayload)
    {
        // CCM_Payload, content version and encoding goes in envelope
        ccm.payload.content.content_version = GetContentVersion();
        ccm.payload.content.content_encoding = used_encoding;
    }
    else
    {
        // not CCM_Payload, content version and encoding goes in header
        ccm.header.content_version = GetContentVersion();
        ccm.header.content_encoding = used_encoding;
    }

    ccm.payload.content.content = std::make_shared<std::vector<unsigned char>>();

    ccm.payload.content.content->assign(static_cast<char*>(encode_buffer.data),
                                             static_cast<char*>(encode_buffer.data) + encode_buffer.num_bytes);

    // free asn1c struct
    ASN_STRUCT_FREE(*type_descriptor, asn1c_packed_payload);

    return true;
}

fs_Encoding CCMEncodable::GetPreferedEncoding ()
{
    return FS_ENCODING_UPER;
}

int CCMEncodable::GetEncodeFlags () const
{
    return kEncodeAsPayload;
}

/// PayloadInterface implementation below

PayloadInterface::ExtendedPayloadIdentifier CCM::GetIdentifier() const
{
    ExtendedPayloadIdentifier payload_identifier = {};

    // legacy signals were not written with const correctness in mind.
    // in practice this is typically const operation, but as there are
    // virtual functions...
    // if any issues related to this are seen the legacy interfaces should
    // be updated.
    CCM* this_not_const = const_cast<CCM*>(this);
    CCMEncodable* ccm_encodable = nullptr;

    ccm_encodable = dynamic_cast<CCMEncodable*>(this_not_const);
    if (ccm_encodable)
    {
        const char* oid = ccm_encodable->GetOid();
        size_t oid_length = std::strlen(oid);

        oid_length = oid_length <= FS_OID_MAX_LEN ? oid_length : FS_OID_MAX_LEN;

        payload_identifier.payload_identifier.oid = std::string(oid, oid_length);
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,
                       "PayloadIdentifier for CCM which is not CCMEncodable requested.");
    }

    return payload_identifier;
}

std::vector<PayloadInterface::CodecType> CCM::GetSupportedCodecs() const
{
    std::vector<CodecType> supported_codecs = {kCodecTypeLegacyCcm};
    return supported_codecs;
}

bool CCM::SetPayload(const CodecPayload& codec_payload)
{
    // Not supported for CcmDecodable
    // Legacy decode has a special path in singal factory
    // as legacy Signals are not default constructable.
    return false;
}

bool CCM::GetPayload(CodecPayload& codec_payload) const
{
    bool return_value = true;

    codec_payload.codec_data.ccm_data.ccm = new ccm_Message();
    ccm_Message* ccm = codec_payload.codec_data.ccm_data.ccm;
    CCMEncodable* ccm_encodable = nullptr;

    if (!ccm)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                       "Failed to allocate ccm_Message.");
        return_value = false;
    }

    if (return_value)
    {
        // legacy signals were not written with const correctness in mind.
        // in practice this is typically const operation, but as there are
        // virtual functions...
        // if any issues related to this are seen the legacy interfaces should
        // be updated.
        CCM* this_not_const = const_cast<CCM*>(this);
        ccm_encodable = dynamic_cast<CCMEncodable*>(this_not_const);
    }

    if (!ccm_encodable)
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                       "Asked to encode CCM which is not CCMEncodable");

        return_value = false;
    }

    if (return_value)
    {
        ExtendedPayloadIdentifier payload_identifier = GetIdentifier();

        std::shared_ptr<std::vector<unsigned char>> encoded_payload = nullptr;

        return_value = timespec_get(&ccm->header.timestamp, TIME_UTC) != 0;

        if (return_value)
        {
            return_value = ccm_SetOid(&(ccm->payload.content.oid),
                                      payload_identifier.payload_identifier.oid.c_str());
        }

        // encode payload, and store it in the ccm_Message
        if (return_value)
        {
            // if we do this after EncodePayload we may overwrite values which it sets
            ccm->header = message->header;

            return_value = ccm_encodable->EncodePayload(*ccm);
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "Failed to set CCMEncodable oid.");
        }

        if (return_value)
        {
            codec_payload.codec_type = kCodecTypeLegacyCcm;
            codec_payload.codec_data.ccm_data.recipients = ccm_encodable->GetRecipients();
            codec_payload.encode_data.encode_flags = ccm_encodable->GetEncodeFlags();
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "Failed to encode CCMEncodable payload.");
        }
    }

    return return_value;
}

bool CCM::GetEmptyPayload(CodecPayload& codec_payload) const
{

    codec_payload.codec_type = kCodecTypeLegacyCcm;

    // legacy signals were not written with const correctness in mind.
    // in practice this is typically const operation, but as there are
    // virtual functions...
    // if any issues related to this are seen the legacy interfaces should
    // be updated.
    CCM* this_not_const = const_cast<CCM*>(this);
    CCMEncodable* ccm_encodable = nullptr;

    ccm_encodable = dynamic_cast<CCMEncodable*>(this_not_const);
    if (ccm_encodable)
    {
        codec_payload.codec_data.ccm_data.recipients = ccm_encodable->GetRecipients();
        codec_payload.encode_data.encode_flags = ccm_encodable->GetEncodeFlags();
    }
    else
    {
        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,
                       "Empty payload for CCM which is not CCMEncodable requested.");
    }

    return true;
}

} // namespace fsm
/** \}    end of addtogroup */
