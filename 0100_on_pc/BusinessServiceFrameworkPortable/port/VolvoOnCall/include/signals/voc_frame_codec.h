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
 *  \file     voc_frame_codec.h
 *  \brief    VOC Service Voc Frame coder encoder.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_VOC_FRAME_CODEC_H_
#define VOC_SIGNALS_VOC_FRAME_CODEC_H_

#include <openssl/evp.h>

#include "voc_framework/signals/signal.h"

#include "signals/voc_frame_encodable_interface.h"
#include "voc.h"

namespace volvo_on_call
{

/**
 * \brief VocFrameCodec codes and decodes Voc Frames, also encrypting and decrypting
 *        as required based on the frame type.
 */
class VocFrameCodec
{
 public:

    /**
     * \brief Constructs a new VocFrameCodec.
     */
    VocFrameCodec();

    /**
     * \brief Encode, and possibly encrypt based on frame type, a Voc Frame.
     *        Frames which should be encrypted require that SetEncryptionKeys
     *        has been successfully called prior to encode.
     *
     * \param[in,out] buffer Encoded bytes will be appended to buffer.
     *                       Note, buffer may be modified even if operation fails.
     * \param[in]     frame  The frame to encode.
     *
     * \return True if succesfull, otherwise false.
     */
    bool Encode(std::vector<unsigned char>& buffer, const VocFrameEncodableInterface& frame);

    // TODO: in signal refactor Signal here becomes "Payload"
    //       it is signal for SignalType...
    /**
     * \brief Decode, and possibly decrypt based on frame type, a Voc Frame.
     *        Frames which should be decrypted require that SetEncryptionKeys
     *        has been successfully called prior to decode.
     *
     * \param[in] buffer Buffer containing the data to decode.
     *
     * \return Valid pointer to a Signal instance (type depending on frame type)
     *         on success, nullptr on failure.
     */
    std::shared_ptr<fsm::Signal> Decode(std::vector<unsigned char>& buffer);

    // TODO: consider spliitng in 3 calls so they can be set as they become available?
    /**
     * \brief Set keys needed for encryption/decryption of voc frames.
     *        The AES XYZ frame types are encrypted/decrypted with a key calculated from:
     *        - TCAM ECDHE public key
     *        - Peer (device) ECDHE public key
     *        - ECDHE shared secret
     *
     * \param[in] public_tcam_key The TCAM public ECDHE key.
     * \param[in] public_peer_key The peers public ECDHE key.
     * \param[in] secret          The ECDHE shared secret.
     *
     * \return True on success, false on failure.
     */
    bool SetEncryptionKeys(EVP_PKEY* public_tcam_key,
                           EVP_PKEY* public_peer_key,
                           const std::vector<unsigned char>& secret);

 private:

    /**
     * \brief index of the identifier byte in the Voc Frame header.
     */
    static const int kVocFrameHeaderIdentifier = 0;
    /**
     * \brief index of the payload length most significant byte in the Voc Frame header.
     */
    static const int kVocFrameHeaderPayloadLengthHighByte = 1;
    /**
     * \brief index of the payload length least significant byte in the Voc Frame header.
     */
    static const int kVocFrameHeaderPayloadLengthLowByte = 2;
    /**
     * \brief index of the frame type byte in the Voc Frame header.
     */
    static const int kVocFrameHeaderFrameType = 3;

    /**
     * \brief Size of the GCM Authentication tag in AES frame types.
     */
    static const int kAuthenticationTagSize = 16;
    /**
     * \brief Size of the initialization vector in AES frame types.
     */
    static const int kInitializationVectorSize = 16;
    /**
     * \brief Fixed size array holding a initialization vector.
     */
    typedef std::array<unsigned char, kInitializationVectorSize> InitializationVector;

    /**
     * \brief Size of the encryption key used with AES frame types.
     */
    static const int kEncryptionKeySize = 16;
    /**
     * \brief Tracks whether a valid encryption key has been generated.
     */
    bool has_valid_encryption_key_ = false;
    //TODO: never goes outside this class and openssl is c style
    //      still consider using a c++ STL container
    /**
     * \brief Holds the encryption key used by this codec.
     */
    unsigned char encryption_key_[kEncryptionKeySize] = {0};

    /**
     * \brief Appends a public key to an in progress hashing operation.
     *        Used when generating the encryption key which is based on
     *        a hash of the shared secret + tcam pubkey + peer pubkey.
     *
     * \param[in] context Context for the ongoing hashing operation.
     * \param[in] key     The public key to append.
     *
     * \return True on success false on failure.
     */
    bool AppendPublicKey(EVP_MD_CTX* context, EVP_PKEY* key);

    /**
     * \brief Check if a frame type should be encrypted / decrypted.
     *
     * \param[in] frame_type The type to check.
     *
     * \return True if the frame type is encrypted,
     *         false if it is not, or if the frame type was unknown.
     */
    bool IsEncryptedFrameType(VocFrameType frame_type);

    /**
     * \brief Encrypts a Voc Frame. Keys must have been set up prior to calling this.
     *
     * \param[in,out] encrypted_data Encrypted bytes will be appended to this.
     * \param[in]     data           The data to encrypt.
     *
     * \return True on success false on failure.
     */
    bool EncryptVocFrame(std::vector<unsigned char>& encrypted_data,
                         const std::vector<unsigned char>& data);

    /**
     * \brief Decrypts a Voc Frame. Keys must have been set upo prior to calling this.
     *
     * \param[in,out] data           Decrypted bytes will be appended to this.
     * \param[in]     encrypted_data The data to decrypt.
     *
     * \return True on success false on failure.
     */
    bool DecryptVocFrame(std::vector<unsigned char>& data,
                         std::vector<unsigned char>& encrypted_data);

    /**
     * \brief Generated a random initialization vector.
     *
     * \param[in,out] initialization_vector Generated initialization vector will be written to this.
     *
     * \return True on success false on failure.
     */
    bool GenerateInitializationVector(InitializationVector& initialization_vector);
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_VOC_FRAME_CODEC_H_

/** \}    end of addtogroup */
