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
 *  \file     aes_csr_voc_frame.cc
 *  \brief    VOC Service AES CSR Voc Frame base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/aes_csr_voc_frame.h"

#include <openssl/evp.h>
#include <openssl/err.h>

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

X509_REQ* AesCsrVocFrame::GetRequest()
{
    return request_;
}

AesCsrVocFrame::~AesCsrVocFrame()
{
    if (request_)
    {
        X509_REQ_free(request_);
    }
}

#ifdef VOC_TESTS

AesCsrVocFrame::AesCsrVocFrame(X509_REQ* request)
{
    if (request)
    {
        request_ = request;
        CRYPTO_add(&((*request_).references), 1, CRYPTO_LOCK_X509_REQ);
    }
}

// ***
// VocFrameEncodableInterface implementation start.
// ***

bool AesCsrVocFrame::GetPayload(std::vector<unsigned char>& payload) const
{
    bool return_value = true;

    if (request_)
    {
        //first get the required buffer size
        int der_buffer_length = i2d_X509_REQ(request_, nullptr);
        int payload_initial_size = payload.size();

        if (der_buffer_length > 0)
        {
            payload.resize(der_buffer_length + payload_initial_size);

            unsigned char* buffer = &(payload[payload_initial_size]);
            int num_encoded_bytes = i2d_X509_REQ(request_, &buffer);

            if (num_encoded_bytes <= 0)
            {
                char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to encode CSR, error: %s",
                                error_buffer);

                return_value = false;
            }
        }
        else
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to get required size to encode CSR, error: %s",
                            error_buffer);

            return_value = false;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Asked to encode NULL CSR.");

        return_value = false;
    }
    return return_value;
}

// ***
// VocFrameEncodableInterface implementation end.
// ***

#endif

// ***
// Protected functions
// ***

AesCsrVocFrame::AesCsrVocFrame()
{
}

bool AesCsrVocFrame::SetPayload(std::vector<unsigned char>& payload)
{
    bool return_value = true;

    if (request_)
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
        if (!d2i_X509_REQ(&request_, &buffer, payload.size()))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to decode CSR, error: %s",
                            error_buffer);

            return_value = false;
        }
    }

    return return_value;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
