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
 *  \file     marben_codec.h
 *  \brief    Codec Interface realization for Marben.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_MARBEN_CODEC_H_
#define VOC_FRAMEWORK_SIGNALS_MARBEN_CODEC_H_

#include "voc_framework/signals/codec_interface.h"

namespace fsm
{

/**
 * \brief Marbencodec wraps the marben runtime and provides a
 *        CodecInterface for asn.1 decode/encode.
 */
class MarbenCodec : public CodecInterface
{

 public:

    /**
     * \brief Decode some encoded data into a PayloadInterface instance.
     *        Will try to create Payload instance if not provided, this
     *        is only possible if a matching PayloadCreator or legacy
     *        SignalFactoryFunction is registered in SignalFactory.
     *
     * \param [out] payload      PayloadInterface instance to populate.
     *                           If nullpointer MarbenCoded will not try to create
     *                           the payload.
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
     * \param [out] applied_encode_flags If not nullptr, will be set 0.
     * \param [out] used_encoding        If not nullptr will be set to hte used asn.1
     *                                   encoding rule.
     * \param [in]  payload              PayloadInterface instance to encode.
     *                                   Must point to a valid PayloadInterface object.
     *
     * \return False on failure, otherwise true.
     */
    bool Encode(std::shared_ptr<std::vector<unsigned char>>& encoded_data,
                fs_Encoding* used_encoding,
                PayloadInterface::EncodeFlags* applied_encode_flags,
                std::shared_ptr<const PayloadInterface> payload) const;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_MARBEN_CODEC_H_

/** \}    end of addtogroup */
