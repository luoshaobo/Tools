/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     xml_codec.cc
 *  \brief    Codec Interface realization for Xml.
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "dlt/dlt.h"

#include "voc_framework/signals/xml_codec.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

bool XmlCodec::Decode(std::shared_ptr<PayloadInterface>& payload,
                         const unsigned char* encoded_data,
                         size_t num_bytes,
                         const fs_Encoding* encoding,
                         const fs_VersionInfo* version ) const
{
    bool return_value = true;

    PayloadInterface::CodecPayload codec_payload;

    if (!payload)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d asked to decode into unspecified payload",
                        PayloadInterface::kCodecTypeXml);
        return_value = false;
    }
    else if (!encoded_data || num_bytes == 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d asked to decode empty data buffer",
                        PayloadInterface::kCodecTypeXml);

        return_value = false;
    }

    if (!payload->GetEmptyPayload(codec_payload))
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "XmlCodec::Encode: Codec for codectype: %d failed to get codec_payload during decode",
                        PayloadInterface::kCodecTypeXml);

        return_value = false;
    }
    else if (codec_payload.codec_type != PayloadInterface::kCodecTypeXml)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "XmlCodec::Encode: Codec for codectype: %d codec type mismatch during decode",
                        PayloadInterface::kCodecTypeXml);

        return_value = false;
    }
    else if (version && codec_payload.version != *version)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "XmlCodec::Encode: Codec for codectype: %d version mismatch during decode",
                        PayloadInterface::kCodecTypeXml);

        return_value = false;
    }

    if (return_value)
    {
        //TODO: We just have plain XML support now, it required in the future we may add
        //      support for handling different encodings. As it should not change the API
        //      on the library user side, we are not introducing any code for this at the moment.

        codec_payload.codec_data.xml_data.data.assign(reinterpret_cast<const char*>(encoded_data), num_bytes);
        return_value = payload->SetPayload(codec_payload);
    }

    return return_value;
}

bool XmlCodec::Encode(std::shared_ptr<std::vector<unsigned char>>& encoded_data,
                         fs_Encoding* used_encoding,
                         PayloadInterface::EncodeFlags* applied_encode_flags,
                         std::shared_ptr<const PayloadInterface> payload) const
{
    bool return_value = true;

    if (payload)
    {
        PayloadInterface::CodecPayload packed_payload;
        payload->GetPayload(packed_payload);

        if (packed_payload.codec_type == PayloadInterface::kCodecTypeXml)
        {
            encoded_data = std::make_shared<std::vector<unsigned char>>(packed_payload.codec_data.xml_data.data.begin(),
                                                                        packed_payload.codec_data.xml_data.data.end());

        }
        else
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "XmlCodec::Encode: Codec for codectype: %d bad codec_data during encode",
                            PayloadInterface::kCodecTypeXml);

            return_value = false;
        }

        if (return_value && encoded_data->size() > 0)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                            "XmlCodec::Encode: Encoded %d bytes",
                            encoded_data->size());
        }
        else if (return_value)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                            "XmlCodec::Encode: Codec for codectype: %d produced buffer was zero size",
                            PayloadInterface::kCodecTypeXml);
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "XmlCodec::Encode: Codec for codectype: %d asked to encode with bad input",
                        PayloadInterface::kCodecTypeXml);

        return_value = false;
    }

    return return_value;
}

} // namespace fsm

/** \}    end of addtogroup */
