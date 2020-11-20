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
 *  \file     voc_frame_codec.cc
 *  \brief    VOC Service Voc Frame coder encoder.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/voc_frame_codec.h"

#include <algorithm>

#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include "dlt/dlt.h"

#include "signals/aes_ble_da_key_voc_frame.h"
#include "signals/aes_cert_actor_voc_frame.h"
#include "signals/aes_cert_nearfield_voc_frame.h"
#include "signals/aes_ca_voc_frame.h"
#include "signals/aes_car_actor_cert_voc_frame.h"
#include "signals/aes_csr_actor_voc_frame.h"
#include "signals/aes_csr_nearfield_voc_frame.h"
#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/ecdhe_request_voc_frame.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

VocFrameCodec::VocFrameCodec()
{
    // ensure PRNG is seeded, we will use it for
    // initialization vectors
    //TODO: is this good? or should we settle for
    //      seed at program start?
    if (RAND_load_file("/dev/urandom", kRndGenSeedSize) != kRndGenSeedSize)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN,
                       "VocFrameCodec, failed to seed PRNG.");
    }
}

bool VocFrameCodec::Encode(std::vector<unsigned char>& buffer, const VocFrameEncodableInterface& frame)
{

    bool return_value = false;

    VocFrameType frame_type = frame.GetFrameType();

    unsigned int buffer_size_before = buffer.size();

    buffer.push_back(kVocFrameIdentifier);
    buffer.push_back(0x0); //placeholder for payload length
    buffer.push_back(0x0); //placeholder for payload length
    buffer.push_back(frame_type);

    if (IsEncryptedFrameType(frame_type))
    {
        std::vector<unsigned char> local_buffer;
        return_value = frame.GetPayload(local_buffer);

        if (return_value)
        {
            // encrypt encode buffer into buffer
            return_value = EncryptVocFrame(buffer, local_buffer);
        }
    }
    else
    {
        return_value = frame.GetPayload(buffer);
    }

    if (return_value)
    {
        unsigned short payload_length =
            buffer.size() - buffer_size_before - kVocFrameHeaderSize;

        unsigned char payload_length_low_byte = (payload_length >> 0) & 0xFF;
        unsigned char payload_length_high_byte = (payload_length >> 8) & 0xFF;

        buffer[buffer_size_before + kVocFrameHeaderPayloadLengthHighByte] =
            payload_length_high_byte;
        buffer[buffer_size_before + kVocFrameHeaderPayloadLengthLowByte] =
            payload_length_low_byte;
    }

    return return_value;
}

// TODO: in signal refactor Signal here becomes "Payload"
//       it is signal for SignalType...
std::shared_ptr<fsm::Signal> VocFrameCodec::Decode(std::vector<unsigned char>& buffer)
{
    bool success = true;

    std::vector<unsigned char> local_buffer = {};
    std::vector<unsigned char>& decode_buffer = local_buffer;

    std::shared_ptr<fsm::Signal> return_pointer;

    VocFrameType frame_type = VocFrameType::kUndefined;

    if (buffer.size() < kVocFrameHeaderSize)
    {
        success = false;
    }
    else if (buffer[kVocFrameHeaderIdentifier] != kVocFrameIdentifier)
    {
        success = false;
    }
    else
    {
        frame_type = static_cast<VocFrameType>(buffer[kVocFrameHeaderFrameType]);

        unsigned char payload_length_low_byte = buffer[kVocFrameHeaderPayloadLengthLowByte];
        unsigned char payload_length_high_byte = buffer[kVocFrameHeaderPayloadLengthHighByte];
        unsigned short payload_length = payload_length_high_byte << 8 | payload_length_low_byte;

        // TODO: erase is quite expensive, if it ever becomes an issue
        //       consider working with iterators in VocFrame interfaces
        buffer.erase(buffer.begin(), buffer.begin() + kVocFrameHeaderSize);

        if (buffer.size() < payload_length)
        {
            success = false;
        }
        else if (buffer.size() > payload_length)
        {
            buffer.erase(buffer.begin() + payload_length, buffer.end());
        }

        if (IsEncryptedFrameType(frame_type))
        {
            // decrypt buffer into decode buffer
            success = DecryptVocFrame(decode_buffer, buffer);
        }
        else
        {
            decode_buffer = buffer;
        }
    }

    if (success)
    {
        switch (frame_type)
        {
        case kEcdheRequest :
            return_pointer =
                EcdheRequestVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kEcdheApproval :
            return_pointer =
                EcdheApprovalVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kAesCsrActor :
            return_pointer =
                AesCsrActorVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kAesCsrNearfield :
            return_pointer =
                AesCsrNearfieldVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
#ifdef VOC_TESTS
        // These are only encodable in production
        case kAesCertActor :
            return_pointer =
                AesCertActorVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kAesCertNearfield :
            return_pointer =
                AesCertNearfieldVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kAesCa :
            return_pointer =
                AesCaVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kAesBleDaKey :
            return_pointer =
                AesBleDaKeyVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
        case kAesCarActorCert :
            return_pointer =
                AesCarActorCertVocFrame::CreateVocFrameSignal(decode_buffer);
            break;
#endif
        default :
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                            "Unsupported Voc Frame frame type received: %d.", frame_type);
        }
    }
    return return_pointer;
}

bool VocFrameCodec::SetEncryptionKeys(EVP_PKEY* public_tcam_key,
                                      EVP_PKEY* public_peer_key,
                                      const std::vector<unsigned char>& secret)
{
    bool return_value = true;

    if (!public_tcam_key || !public_peer_key || !(secret.size() > 0))
    {
        return_value = false;
    }

    unsigned char checksum[EVP_MAX_MD_SIZE] = {};
    unsigned int checksum_length = 0;

    EVP_MD_CTX* context = EVP_MD_CTX_create();
    if (!context)
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "VocFrameCodec, failed to create digest context, error: %s",
                        error_buffer);
        return_value = false;
    }

    if (return_value && !EVP_DigestInit_ex(context, EVP_sha256(), NULL))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "VocFrameCodec, failed to initialize digest context, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && !EVP_DigestUpdate(context, &(secret[0]), secret.size()))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "VocFrameCodec, failed to add secret to digest, error: %s",
                        error_buffer);
        return_value = false;
    }

    if (return_value)
    {
        return_value = AppendPublicKey(context, public_tcam_key);
    }
    if (return_value)
    {
        return_value = AppendPublicKey(context, public_peer_key);
    }

    if (return_value && !EVP_DigestFinal_ex(context, checksum, &checksum_length))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                        "VocFrameCodec, failed to finalize digest, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (checksum_length < kEncryptionKeySize)
    {
        return_value = false;
    }

    if (return_value)
    {
        memcpy(encryption_key_, checksum, kEncryptionKeySize);
        has_valid_encryption_key_ = true;
    }

    EVP_MD_CTX_destroy(context);

    return return_value;
}

bool VocFrameCodec::AppendPublicKey(EVP_MD_CTX* context, EVP_PKEY* key)
{
    bool return_value = true;

    EC_KEY* ec_key = EVP_PKEY_get1_EC_KEY(key);

    if (ec_key)
    {
        unsigned char* buffer = nullptr;

        int buffer_size = i2o_ECPublicKey(ec_key, &(buffer));
        if (buffer_size < 0)
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "VocFrameCodec, bad key, error: %s",
                            error_buffer);
            return_value = false;
        }
        if (return_value && !EVP_DigestUpdate(context, buffer, buffer_size))
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR,
                            "VocFrameCodec, failed to add key to digest, error: %s",
                            error_buffer);
            return_value = false;
        }

        if (buffer)
        {
            OPENSSL_free(buffer);
        }

        EC_KEY_free(ec_key);
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

bool VocFrameCodec::IsEncryptedFrameType(VocFrameType frame_type)
{
    bool is_encrypted_frame_type = false;

    switch (frame_type)
    {
    case kEcdheRequest :
    case kEcdheApproval :
        is_encrypted_frame_type = false;
        break;
    case kAesCsrActor :
    case kAesCsrNearfield :
    case kAesCertActor :
    case kAesCertNearfield :
    case kAesCa :
    case kAesBleDaKey :
    case kAesCarActorCert :
        is_encrypted_frame_type = true;
        break;
    default:
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN,
                        "Unsupported Voc Frame frame type received: %d.", frame_type);
    }
    return is_encrypted_frame_type;
}

bool VocFrameCodec::EncryptVocFrame(std::vector<unsigned char>& encrypted_data,
                                    const std::vector<unsigned char>& data)
{
    bool return_value = true;

    unsigned char* authentication_tag = nullptr;
    unsigned char* encrypted_data_buffer = nullptr;

    InitializationVector initialization_vector;

    if (!has_valid_encryption_key_)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "VocFrameCodec::EncryptVocFrame no encryption key-");
        return_value = false;
    }

    if (data.size() == 0)
    {
        return_value = false;
    }

    // generate initialization_vector
    if (return_value)
    {
        return_value = GenerateInitializationVector(initialization_vector);
    }

    if (return_value)
    {
        // write initialization vector to encrypted data
        std::copy(initialization_vector.begin(),
                  initialization_vector.end(),
                  std::back_inserter(encrypted_data));

        int authentication_tag_start = encrypted_data.size();

        // reserve room for authentication tag
        std::fill_n(std::back_inserter(encrypted_data),
                    kAuthenticationTagSize,
                    0);

        int encrypted_data_start = encrypted_data.size();

        // reserve room for encrypted data, we will have openssl write directly into the vector
        // as we are using GCM size of encrypted data = size of data
        std::fill_n(std::back_inserter(encrypted_data),
                    data.size(),
                    0);

        // pointers into buffer
        authentication_tag = &(encrypted_data[authentication_tag_start]);
        encrypted_data_buffer = &(encrypted_data[encrypted_data_start]);
    }

    // set up encryption context
    EVP_CIPHER_CTX *ctx = nullptr;
    if (return_value)
    {
        ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to get cipher context, error: %s",
                            error_buffer);
            return_value = false;
        }
    }
    if (return_value && !EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to init cipher context, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && !EVP_CIPHER_CTX_ctrl(ctx,
                                             EVP_CTRL_GCM_SET_IVLEN,
                                             kInitializationVectorSize,
                                             NULL))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to set iv len, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && !EVP_EncryptInit_ex(ctx,
                                            NULL,
                                            NULL,
                                            &(encryption_key_[0]),
                                            &(initialization_vector[0])))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to finalize init of cipher context, error: %s",
                        error_buffer);
        return_value = false;
    }

    // encrypt
    int num_encrypted_bytes = 0;

    if (return_value && !EVP_EncryptUpdate(ctx,
                                          encrypted_data_buffer,
                                          &num_encrypted_bytes,
                                          &(data[0]),
                                          data.size()))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to encrypt, error: %s",
                        error_buffer);
        return_value = false;
    }

    if (return_value && !EVP_EncryptFinal_ex(ctx,
                                             encrypted_data_buffer + num_encrypted_bytes,
                                             &num_encrypted_bytes))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to finish encryption, error: %s",
                        error_buffer);
        return_value = false;
    }

    if(return_value && ! EVP_CIPHER_CTX_ctrl(ctx,
                                            EVP_CTRL_GCM_GET_TAG,
                                            kAuthenticationTagSize,
                                            authentication_tag))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to get authentication tag, error: %s",
                        error_buffer);
        return_value = false;
    }

    EVP_CIPHER_CTX_free(ctx);

    return return_value;
}

bool VocFrameCodec::DecryptVocFrame(std::vector<unsigned char>& data,
                                    std::vector<unsigned char>& encrypted_data)
{
    bool return_value = true;

    unsigned char* initialization_vector = &(encrypted_data[0]);
    unsigned char* authentication_tag = &(encrypted_data[kInitializationVectorSize]);
    unsigned char* encrypted_data_buffer = &(encrypted_data[kInitializationVectorSize +
                                                            kAuthenticationTagSize]);
    unsigned char* decrypted_data_buffer = nullptr;

    int num_encrypted_bytes = encrypted_data.size() -
                              kInitializationVectorSize -
                              kAuthenticationTagSize;

    if (!has_valid_encryption_key_)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_WARN, "VocFrameCodec::EncryptVocFrame no encryption key-");
        return_value = false;
    }
    else if (num_encrypted_bytes <= 0)
    {
        return_value = false;
    }
    else
    {
        // reserve room for decrypted data
        std::fill_n(std::back_inserter(data),
                    num_encrypted_bytes,
                    0);
        decrypted_data_buffer = &(data[0]);
    }

    // intialize decryption context
    EVP_CIPHER_CTX *ctx = nullptr;
    if (return_value)
    {
        ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to get cipher context, error: %s",
                            error_buffer);
            return_value = false;
        }
    }
    if (return_value && !EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to init cipher context, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && !EVP_CIPHER_CTX_ctrl(ctx,
                                        EVP_CTRL_GCM_SET_IVLEN,
                                        kInitializationVectorSize,
                                        NULL))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to set iv len, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && !EVP_DecryptInit_ex(ctx,
                                       NULL,
                                       NULL,
                                       &(encryption_key_[0]),
                                       initialization_vector))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to finish cipher context init, error: %s",
                        error_buffer);
        return_value = false;
    }

    // decrypt
    int num_decrypted_bytes = 0;
    if (return_value && !EVP_DecryptUpdate(ctx,
                                      decrypted_data_buffer,
                                      &num_decrypted_bytes,
                                      encrypted_data_buffer,
                                      num_encrypted_bytes))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to decrypt, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && !EVP_CIPHER_CTX_ctrl(ctx,
                                        EVP_CTRL_GCM_SET_TAG,
                                        kAuthenticationTagSize,
                                        authentication_tag))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to set authentication token, error: %s",
                        error_buffer);
        return_value = false;
    }
    if (return_value && EVP_DecryptFinal_ex(ctx,
                                       decrypted_data_buffer + num_decrypted_bytes,
                                       &num_decrypted_bytes) <= 0)
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to finalize decryption, error: %s",
                        error_buffer);
        return_value = false;
    }

    EVP_CIPHER_CTX_free(ctx);

    return return_value;
}

bool VocFrameCodec::GenerateInitializationVector(InitializationVector& initialization_vector)
{
    bool return_value = true;

    unsigned char* initialization_vector_buffer = &(initialization_vector[0]);

    if (!RAND_bytes(initialization_vector_buffer, kInitializationVectorSize))
    {
        char* error_buffer = ERR_error_string(ERR_get_error(), NULL);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "Failed to generate initialization_vector, error: %s",
                        error_buffer);

        return_value = false;
    }

    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
