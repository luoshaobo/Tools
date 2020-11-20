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
 *  \file     aes_cert_voc_frame.cc
 *  \brief    VOC Service AES CERT Voc Frame base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/aes_cert_voc_frame.h"

#include <openssl/evp.h>
#include <openssl/err.h>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

AesCertVocFrame::~AesCertVocFrame()
{
    if (certificate_)
    {
        X509_free(certificate_);
    }
}

AesCertVocFrame::AesCertVocFrame(X509* certificate)
{
    if (certificate)
    {
        certificate_ = certificate;
        X509_up_ref(certificate);
    }
}

// ***
// VocFrameEncodableInterface implementation start.
// ***

bool AesCertVocFrame::GetPayload(std::vector<unsigned char>& payload) const
{
    bool return_value = true;

    if (certificate_)
    {
        //first get the required buffer size
        int der_buffer_length = i2d_X509(certificate_, nullptr);
        int payload_initial_size = payload.size();

        if (der_buffer_length > 0)
        {
            payload.resize(der_buffer_length + payload_initial_size);

            unsigned char* buffer = &(payload[payload_initial_size]);
            int num_encoded_bytes = i2d_X509(certificate_, &buffer);

            if (num_encoded_bytes <= 0)
            {
                char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to encode CERT, error: %s",
                                error_buffer);

                return_value = false;
            }
        }
        else
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to get required size to encode CERT, error: %s",
                            error_buffer);

            return_value = false;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Asked to encode NULL CERT.");

        return_value = false;
    }

    return return_value;
}

// ***
// VocFrameEncodableInterface implementation end.
// ***

#ifdef VOC_TESTS

X509* AesCertVocFrame::GetCertificate()
{
    return certificate_;
}

// ***
// Protected functions
// ***

AesCertVocFrame::AesCertVocFrame()
{
}

bool AesCertVocFrame::SetPayload(std::vector<unsigned char>& payload)
{
    bool return_value = true;

    if (certificate_)
    {
        return_value = false;
    }
    else if (payload.size() == 0)
    {
        return_value = false;
    }
    else
    {
        const unsigned char* buffer = &(payload[0]);
        if (!d2i_X509(&certificate_, &buffer, payload.size()))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to decode CERT, error: %s",
                            error_buffer);

            return_value = false;
        }
    }

    return return_value;
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
