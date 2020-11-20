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
 *  \file     ccm_codec.h
 *  \brief    Codec Interface realizatoin for CCM.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CCM_CODEC_H_
#define VOC_FRAMEWORK_SIGNALS_CCM_CODEC_H_

#include "voc_framework/signals/codec_interface.h"

namespace fsm
{

/**
 * \brief CcmCodec wraps fsm_ccm and provides a CodecInterface for
 *        CCM decode/encode.
 *        The codec is aware of both CCMDecodable/CCMEncodable and
 *        CcmSignal + PayloadInterface signal implementations and is
 *        capable od crating both.
 */
class CcmCodec : public CodecInterface
{

 public:
    /**
     * \brief Converts the internal Encode flags to CCM encode flags.
     *        The mentioned CCM encode flags are part of FSM CCM interface.
     *
     * \param[in] encodeFlags VoC-side EncodeFlag(s)
     *
     * \return CCM encode flags for FSM CCM interface.
     */
    static int GetCcmEncodeFlags(PayloadInterface::EncodeFlags encodeFlags);

    /**
     * \brief Decode some encoded data into a PayloadInterface instance.
     *        Will try to create Payload instance if not provided, this
     *        is only possible if a matching PayloadCreator or legacy
     *        SignalFactoryFunction is registered in SignalFactory.
     *
     * \param [out] payload      PayloadInterface instance to populate.
     *                           If nullpointer the codec will try to create
     *                           the payload. Otherwise the provided payload
     *                           will be populated.
     * \param [in]  encoded_data Data to decode. Must be a CCM.
     * \param [in]  num_bytes    Size of data to decode.
     * \param [in]  encoding     Ignored.
     * \param [in]  version      Ignored.
     *
     * \return False on failure, otherwise true.
     */
    bool Decode(std::shared_ptr<PayloadInterface>& payload,
                const unsigned char* encoded_data,
                size_t num_bytes,
                const fs_Encoding* encoding = nullptr,
                const fs_VersionInfo* version = nullptr) const;

    /**
     * \brief Encode a PayloadInterface instance.
     *
     * \param [out] encoded_data         Will be populated with encoded data.
     *                                   Must point to a valid vector.
     * \param [out] applied_encode_flags If not nullptr, will be set to inform which encode
     *                                   flags, if any, were applied during encode.
     * \param [out] used_encoding        If not null will be set to FS_ENCODING_UNDEFINED,
     * \param [in]  payload              PayloadInterface instance to encode.
     *                                   Must point to a valid PayloadInterface object.
     *
     * \return False on failure, otherwise true.
     */
    bool Encode(std::shared_ptr<std::vector<unsigned char>>& encoded_data,
                fs_Encoding* used_encoding,
                PayloadInterface::EncodeFlags* applied_encode_flags,
                std::shared_ptr<const PayloadInterface> payload) const;

 private:
    /**
     * \brief Attempt to create a legacty ccm signal from a decoded ccm.
     *
     * \param[in] identifier     identifier
     * \param[in] message        decoded message
     * \param[in] transaction_id transaction_id of message
     * \param[in] sender         sender of message, takes ownership of pointer
     * \param[in] certificates   certificates attached to message
     *
     * \return nullptr on failure, or shared ptr to created signal.
     */
    std::shared_ptr<PayloadInterface> CreateLegacyCcm(PayloadInterface::PayloadIdentifier identifier,
                                                       ccm_Message* message,
                                                       CCMTransactionId& transaction_id,
                                                       fsm::UserId sender,
                                                       STACK_OF(X509)* certificates) const;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CCM_CODEC_H_

/** \}    end of addtogroup */
