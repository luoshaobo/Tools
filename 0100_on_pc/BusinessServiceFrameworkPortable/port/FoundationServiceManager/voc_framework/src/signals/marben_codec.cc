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
 *  \file     marben_codec.cc
 *  \brief    Codec Interface realization for Marben.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "dlt/dlt.h"

#include "voc_framework/signals/marben_codec.h"

DLT_IMPORT_CONTEXT(dlt_libfsm);

namespace fsm
{

bool MarbenCodec::Decode(std::shared_ptr<PayloadInterface>& payload,
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
                        "Codec for codectype: %d asked to decode into unspecified payload\n",
                        PayloadInterface::kCodecTypeMarben);

        // marben has some suport to try and discover the type of BER/DER and XER CXER data
        // this could be experimented with in future, but for now MarbeNCodec will not try
        // to create payloads
        return_value = false;
    }
    else if (!encoded_data || num_bytes == 0)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d asked to decode empty data buffer\n",
                        PayloadInterface::kCodecTypeMarben);

        return_value = false;
    }

    if (!payload->GetEmptyPayload(codec_payload))
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d failed to get codec_payload during decode\n",
                        PayloadInterface::kCodecTypeMarben);

        return_value = false;
    }
    else if (codec_payload.codec_type != PayloadInterface::kCodecTypeMarben)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d codec type mismatch during decode\n",
                        PayloadInterface::kCodecTypeMarben);

        return_value = false;
    }
    else if (version && codec_payload.version != *version)
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d version mismatch during decode\n",
                        PayloadInterface::kCodecTypeMarben);

        return_value = false;
    }

    if (return_value)
    {
        //TODO: optimization, keep one context per thread
        //      in instance data. They may not be shared acress threads.
        asnContext context;

        fs_Encoding local_encoding = FS_ENCODING_UPER; //UPER is default so lets try it if none given
        if (encoding)
        {
            local_encoding = *encoding;
        }

        // build a decoding stream over the buffer
        asnMemoryStream decoding(const_cast<unsigned char*>(encoded_data), //marben does not have const modifier :(
                                 num_bytes,
                                 asnFSTREAM_READ);

        try //Marben throws exceptions...
        {
            switch (local_encoding)
            {
            case FS_ENCODING_BER :
                codec_payload.codec_data.marben_data->BERdecode(&context, &decoding);
                break;
            case FS_ENCODING_DER :
                codec_payload.codec_data.marben_data->DERdecode(&context, &decoding);
                break;
            case FS_ENCODING_PER :
                codec_payload.codec_data.marben_data->PERdecode(&context, &decoding);
                break;
            case FS_ENCODING_UPER :
                codec_payload.codec_data.marben_data->UPERdecode(&context, &decoding);
                break;
            case FS_ENCODING_XER :
                codec_payload.codec_data.marben_data->XERdecode(&context, &decoding);
                break;
            case FS_ENCODING_XML :
            case FS_ENCODING_CER :
            case FS_ENCODING_OER :
            case FS_ENCODING_UNDEFINED :
            default :
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "Codec for codectype: %d unsupported encoding during decode\n",
                                PayloadInterface::kCodecTypeMarben);

                return_value = false;
            }
        }
        catch (const asnException& e)
        {
            DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                            "Codec for codectype: %d codec error during decode\n",
                            PayloadInterface::kCodecTypeMarben);

            return_value = false;
        }
    }

    if (return_value)
    {
        return_value = payload->SetPayload(codec_payload);
    }

    return return_value;
}

bool MarbenCodec::Encode(std::shared_ptr<std::vector<unsigned char>>& encoded_data,
                         fs_Encoding* used_encoding,
                         PayloadInterface::EncodeFlags* applied_encode_flags,
                         std::shared_ptr<const PayloadInterface> payload) const
{
    bool return_value = true;

    if (payload)
    {
        PayloadInterface::CodecPayload packed_payload;
        payload->GetPayload(packed_payload);

        if (packed_payload.codec_type == PayloadInterface::kCodecTypeMarben)
        {
            //TODO: optimization, keep one context per thread
            //      in instance data. They may not be shared acress threads.
            asnContext context;

            asnMemoryStream encoding(asnFSTREAM_WRITE);

            asnMAXUINT buffer_length = 0;

            if (packed_payload.codec_data.marben_data)
            {
                try //Marben throws exceptions...
                {
                    switch (packed_payload.encode_data.prefered_encoding)
                    {
                    case FS_ENCODING_DER :
                        buffer_length = packed_payload.codec_data.marben_data->DERencode(&context,
                                                                                         &encoding);
                        if (used_encoding && buffer_length > 0)
                        {
                            *used_encoding = FS_ENCODING_DER;
                        }
                        break;
                    case FS_ENCODING_PER :
                        buffer_length = packed_payload.codec_data.marben_data->PERencode(&context,
                                                                                          &encoding);
                        if (used_encoding && buffer_length > 0)
                        {
                            *used_encoding = FS_ENCODING_PER;
                        }
                        break;
                    case FS_ENCODING_BER :
                        buffer_length = packed_payload.codec_data.marben_data->BERencode(&context,
                                                                                         &encoding);
                        if (used_encoding && buffer_length > 0)
                        {
                            *used_encoding = FS_ENCODING_BER;
                        }
                        break;
                    case FS_ENCODING_XER :
                        buffer_length = packed_payload.codec_data.marben_data->XERencode(&context,
                                                                                         &encoding);
                        if (used_encoding && buffer_length > 0)
                        {
                            *used_encoding = FS_ENCODING_XER;
                        }
                        break;
                    case FS_ENCODING_UNDEFINED :
                    case FS_ENCODING_UPER :
                        buffer_length = packed_payload.codec_data.marben_data->UPERencode(&context,
                                                                                          &encoding);
                        if (used_encoding && buffer_length > 0)
                        {
                            *used_encoding = FS_ENCODING_UPER;
                        }
                        break;
                    case FS_ENCODING_OER :
                    case FS_ENCODING_CER :
                    case FS_ENCODING_XML :
                    default :
                        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                        "Codec for codectype: %d "
                                        "asked to encode unsupported encoding: %\n",
                                        PayloadInterface::kCodecTypeMarben,
                                        packed_payload.codec_data.marben_data);

                        return_value = false;

                        if (used_encoding)
                        {
                            *used_encoding = FS_ENCODING_UNDEFINED;
                        }
                    }
                }
                catch (const asnException& e)
                {
                    DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "Codec for codectype: %d codec error during encode\n",
                                PayloadInterface::kCodecTypeMarben);

                    return_value = false;

                    if (used_encoding)
                    {
                        *used_encoding = FS_ENCODING_UNDEFINED;
                    }
                }
            }
            else
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                                "Codec for codectype: %d bad codec_data during encode\n",
                                PayloadInterface::kCodecTypeMarben);

                return_value = false;
            }

            if (return_value && buffer_length > 0)
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_INFO,
                                "Encoded %d bytes\n",
                                buffer_length);

                asnbytep buffer = encoding.detach();

                encoded_data = std::make_shared<std::vector<unsigned char>>(buffer,
                                                                            buffer + buffer_length);

                asnfree(buffer);
            }
            else if (return_value)
            {
                DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_WARN,
                                "Codec for codectype: %d produced buffer was zero size\n",
                                PayloadInterface::kCodecTypeMarben);

                encoded_data = std::make_shared<std::vector<unsigned char>>();
            }
        }
        else
        {
            return_value = false;
        }
    }
    else
    {
        DLT_LOG_STRINGF(dlt_libfsm, DLT_LOG_ERROR,
                        "Codec for codectype: %d asked to encode with bad input\n",
                        PayloadInterface::kCodecTypeMarben);

        return_value = false;
    }

    if (return_value && applied_encode_flags)
    {
        *applied_encode_flags = 0;
    }

    return return_value;
}

} // namespace fsm

/** \}    end of addtogroup */
