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
 *  \file     ecdhe_request_voc_frame.cc
 *  \brief    VOC Service AES BL DA KEY Voc Frame.
 *  \author   Imran Siddique
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ecdhe_request_voc_frame.h"

#include <openssl/evp.h>
#include <openssl/err.h>
#include <algorithm>

#include "dlt/dlt.h"

#include "signals/signal_types.h"

#include <iostream>

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

EcdheRequestVocFrame::EcdheRequestVocFrame(EVP_PKEY* key) :
    // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
    fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kEcdheRequestVocFrame),
    key_(key)
{
    //increase key ref counter
    if (key_)
    {
        EVP_PKEY_up_ref(key);
    }
}

// ***
// VocFrameEncodableInterface implementation start.
// ***

VocFrameType EcdheRequestVocFrame::GetFrameType() const
{
    return kEcdheRequest;
}

bool EcdheRequestVocFrame::GetPayload(std::vector<unsigned char>& payload) const
{
    bool return_value = true;

    if (key_)
    {
        EC_KEY* ec_key = EVP_PKEY_get1_EC_KEY(key_);
        if (ec_key)
        {
            unsigned char* buffer = nullptr;

            int buffer_size = i2o_ECPublicKey(ec_key, &(buffer));
            if (!buffer_size)
            {
                char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "EcdheRequestVocFrame, failed to encode key, error: %s",
                                error_buffer);

                return_value = false;
            }
            else
            {
                payload.insert(payload.end(), buffer, buffer+buffer_size);

                return_value = true;
            }
        }
        else
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "EcdheRequestVocFrame, not ec key, error: %s",
                            error_buffer);

            return_value = false;
        }
    }
    else
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "EcdheRequestVocFrame, asked to encode empty frame.");

        return_value = false;
    }

    return return_value;
}

EVP_PKEY* EcdheRequestVocFrame::GetKey()
{
    // increase key ref counter, client should free return handle
    if (key_)
    {
        EVP_PKEY_up_ref(key_);
    }
    return key_;
}

// ***
// VocFrameEncodableInterface implementation end.
// ***

// ***
// Signal implementation start.
// ***

std::string EcdheRequestVocFrame::ToString()
{
    return "EcdheRequestVocFrame";
}

// ***
// Signal implementation end.
// ***

std::shared_ptr<fsm::Signal> EcdheRequestVocFrame::CreateVocFrameSignal(std::vector<unsigned char>& payload)
{
    std::shared_ptr<fsm::Signal> return_pointer = nullptr;
    const unsigned char* buffer = payload.data();

    EVP_PKEY* pkey = EVP_PKEY_new();
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

    EC_KEY* ret_key = o2i_ECPublicKey(&ec_key, &buffer, payload.size());

    if (ret_key != nullptr)
    {
        if (!EVP_PKEY_set1_EC_KEY(pkey, ec_key))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "EcdheRequestVocFrame, failed to set EC key, error: %s",
                            error_buffer);
        }
        else
        {
            return_pointer = std::make_shared<EcdheRequestVocFrame>(pkey);
        }
    }
    else
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "EcdheRequestVocFrame, failed to decode key, error: %s",
                        error_buffer);
    }

    if (ec_key)
    {
        EC_KEY_free(ec_key);
    }
    if (pkey)
    {
        EVP_PKEY_free(pkey);
    }

    return return_pointer;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
