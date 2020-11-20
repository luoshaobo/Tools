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
 *  \file     ccm_codec.cc
 *  \brief    Codec Interface realizatoin for CCM.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "voc_framework/signals/ccm_codec.h"

#include <algorithm>

#include "dlt/dlt.h"

#include "fsm_ccm.h"

#include "voc_framework/signals/ccm.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_signal.h"
#include "voc_framework/signals/payload_interface.h"
#include "voc_framework/signals/signal_factory.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

int CcmCodec::GetCcmEncodeFlags(PayloadInterface::EncodeFlags encode_flags)
{
    int flags = 0;

    flags += encode_flags & PayloadInterface::kEncodeAsPayload ? CCM_ENCODE_AS_PAYLOAD : 0;
    flags += encode_flags & PayloadInterface::kDoNotEncrypt ? CCM_DO_NOT_ENCRYPT : 0;

    return flags;
}

bool CcmCodec::Decode(std::shared_ptr<PayloadInterface>& payload,
                      const unsigned char* encoded_data,
                      size_t num_bytes,
                      const fs_Encoding* encoding,
                      const fs_VersionInfo* version) const
{

    bool return_value = true;

    //output variables to ccm_Decode
    ccm_Message* message = NULL;
    fsm::UserId sender = fsm::kUndefinedUserId;
    STACK_OF(X509)* certificates = nullptr;

    if (!encoded_data || num_bytes == 0)
    {
        return_value = false;
    }

    if (return_value)
    {
        const_fs_Data fs_encoded_data = {encoded_data, num_bytes};
        unsigned int decode_flags = 0;

        if (!ccm_Decode(&fs_encoded_data,
                        &message,
                        sender,
                        &certificates,
                        decode_flags))
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to decode CCM message");

            return_value = false;
        }
    }

    if (return_value && message)
    {
       PayloadInterface::PayloadIdentifier identifier = PayloadInterface::PayloadIdentifier();
       identifier.oid = std::string(message->payload.content.oid.oid);
       CCMTransactionId transaction_id(message->header.transaction_id);

       if (!payload)
       {
          // Create instance to decode into

          // If we can find a PayloadFactoryFunction for the payload we handle the CCM
          // via CcmSignal.

          SignalFactory::PayloadFactoryFunction factory_function;
          factory_function = SignalFactory::GetPayloadFactory(identifier);

          if (factory_function)
          {
             payload = std::make_shared<CcmSignal>(transaction_id);
          }
       }

       // if we now have a payload unpack into it, otherwise attempt legacy CCM.
       if (payload)
       {
          PayloadInterface::CodecPayload codec_payload = PayloadInterface::CodecPayload();
          payload->GetEmptyPayload(codec_payload);
          codec_payload.codec_data.ccm_data.ccm = message; //this gives away ownership of message
          codec_payload.codec_data.ccm_data.sender = sender;

          // If SetPayload below goes well the payload takes ownership of the certificate stack,
          // else we free it before returning.
          codec_payload.codec_data.ccm_data.certificates = certificates;

          return_value = payload->SetPayload(codec_payload);
       }
       else
       {
          // takes ownership of message
          payload = CreateLegacyCcm(identifier, message, transaction_id, sender, certificates);

          if (!payload)
          {
             return_value = false;
          }
       }
    }

    if (!return_value && certificates)
    {
        sk_X509_pop_free(certificates, X509_free);
    }

    return return_value;
}

bool CcmCodec::Encode(std::shared_ptr<std::vector<unsigned char>>& encoded_data,
                      fs_Encoding* used_encoding,
                      PayloadInterface::EncodeFlags* applied_encode_flags,
                      std::shared_ptr<const PayloadInterface> payload) const
{
    bool return_value = true;

    if (payload == nullptr)
    {
        return_value = false;
    }

    if (return_value)
    {
        std::vector<PayloadInterface::CodecType> supported_codecs = payload->GetSupportedCodecs();

        if (std::find_if(supported_codecs.begin(), supported_codecs.end(),
                         [](PayloadInterface::CodecType codec_type)
                         {
                             return codec_type == PayloadInterface::kCodecTypeCcm ||
                                    codec_type == PayloadInterface::kCodecTypeLegacyCcm;
                         })
            != supported_codecs.end())
        {
            PayloadInterface::CodecPayload codec_payload = PayloadInterface::CodecPayload();

            return_value = payload->GetPayload(codec_payload);

            if (return_value)
            {
                fs_Data data = fs_Data();

                return_value = ccm_Encode(&data,
                                          codec_payload.codec_data.ccm_data.ccm,
                                          codec_payload.codec_data.ccm_data.recipients,
                                          GetCcmEncodeFlags(codec_payload.encode_data.encode_flags));

                if (return_value)
                {
                    // TODO: get rid of this memcpy!
                    unsigned char* data_start = static_cast<unsigned char *>(data.data);
                    unsigned char* data_end = data_start + data.num_bytes;
                    encoded_data = std::make_shared<std::vector<unsigned char>>(data_start,
                                                                                data_end);

                    free(data.data);

                    if (!encoded_data)
                    {
                        return_value = false;
                    }
                }
                else
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                                   "Failed to encode CCM.");
                }

                if (return_value && applied_encode_flags)
                {
                    *applied_encode_flags = codec_payload.encode_data.encode_flags;
                }
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN,
                           "CCM Encode on payload which does not support CCM.");

            return_value = false;
        }

    }

    if (return_value && used_encoding)
    {
        *used_encoding = FS_ENCODING_UNDEFINED;
    }

    return return_value;
}

std::shared_ptr<PayloadInterface> CcmCodec::CreateLegacyCcm(PayloadInterface::PayloadIdentifier identifier,
                                                             ccm_Message* message,
                                                             CCMTransactionId& transaction_id,
                                                             fsm::UserId sender,
                                                             STACK_OF(X509)* certificates) const
{
    SignalFactory::SignalFactoryFunction legacy_factory_function;
    legacy_factory_function = SignalFactory::GetLegacySignalFactoryFunction(identifier.oid);

    std::shared_ptr<PayloadInterface> legacy_signal_as_payload = nullptr;

    if (legacy_factory_function)
    {
        std::shared_ptr<Signal> legacy_signal = nullptr;
        legacy_signal = legacy_factory_function(message, transaction_id);

        if (legacy_signal)
        {
            if (sender != fsm::kUndefinedUserId)
            {
                legacy_signal->SetSender(sender);
            }

            std::shared_ptr<CCM> legacy_signal_as_ccm = std::dynamic_pointer_cast<CCM>(legacy_signal);

            if (legacy_signal_as_ccm)
            {
                if (!legacy_signal_as_ccm->SetCertificates(certificates))
                {
                    DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Failed to set certificates in legacy signal.");
                }
            }
            else
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_WARN, "Created legacy signal which is not a CCM.");
            }

            legacy_signal_as_payload = std::dynamic_pointer_cast<PayloadInterface>(legacy_signal);

            if (!legacy_signal_as_payload)
            {
                DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                               "Created legacy signal which is not a PayloadInterface.");
            }
        }
        else
        {
            DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR,
                           "Failed to create legacy signal.");
        }

        //modify klocwork warning,yangjun add 20181107
        if (message)
        {
            delete message;
        }
    }
    else
    {
        // we never passed ownership of the pointer to the ccm
        if (message)
        {
            delete message;
        }

        DLT_LOG_STRING(dlt_libfsm, DLT_LOG_ERROR, "Failed to find legacy signal factory.");
    }

    return legacy_signal_as_payload;
}


} // namespace fsm
/** \}    end of addtogroup */
