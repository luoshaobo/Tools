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
 *  \file     codec_interface.h
 *  \brief    Interface to be realized by payload codec implementations.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_FRAMEWORK_SIGNALS_CODEC_INTERFACE_H_
#define VOC_FRAMEWORK_SIGNALS_CODEC_INTERFACE_H_

#include <memory>
#include <vector>

#include "voc_framework/signals/payload_interface.h"

namespace fsm
{

/**
 * \brief CodecInterface provides functions necessery for SignalFactory to
 *        encode/decode PayloadInterface realizations.
 *
 *        CodecInterface realizations should handle the encode/decode
 *        of data obtained from the PayloadInterface realization via
 *        GetPayload.
 *
 *        Complex nested data should preferebly be handled by nested
 *        PayloadInterfaces with corresponding codecs. For instance the
 *        CCM CodecInterface realization will not directly try to decode
 *        the payload and instead assume that CcmSignal will handle that
 *        in GetPayload.
 */
class CodecInterface
{

 public:

    /**
     * \brief Decode some encoded data into a PayloadInterface instance.
     *        Will try to create Payload instance if not provided, this
     *        is only possible if:
     *        - the encoded_data contains meta data about what payload it
     *          contains such that a PayloadIdentidfier can be populated
     *        - a matching PayloadCreator is registered in SignalFactory
     *
     * \param [in/out] payload   PayloadInterface instance to populate.
     *                           If nullpointer the codec may try to create
     *                           the payload. Otherwise the provided payload
     *                           will be populated.
     * \param [in]  encoded_data Data to decode.
     * \param [in]  num_bytes    Size of data to decode.
     * \param [in]  encoding     Optional information to codec on which encoding
     *                           data is encoded with. Typically used during ccm
     *                           decode.
     * \param [in]  version      Optional information to codec on which version
     *                           of payload the data contains. Typically used during ccm
     *                           decode.
     *
     * \return False on failure, otherwise true.
     */
    virtual bool Decode(std::shared_ptr<PayloadInterface>& payload,
                        const unsigned char* encoded_data,
                        size_t num_bytes,
                        const fs_Encoding* encoding = nullptr,
                        const fs_VersionInfo* version = nullptr) const = 0;

    /**
     * \brief Encode a PayloadInterface instance.
     *
     * \param [out] encoded_data         Will be set to point to a buffer of encoded data.
     * \param [out] used_encoding        If not nullptr, will be set to the encoding which
     *                                   was used to encode the payload.
     * \param [out] applied_encode_flags If not nullptr, will be set to inform which encode
     *                                   flags, if any, were applied during encode.
     * \param [in]  payload              PayloadInterface instance to encode.
     *                                   Must point to a valid PayloadInterface object.
     *
     * \return False on failure, otherwise true.
     */
    virtual bool Encode(std::shared_ptr<std::vector<unsigned char>>& encoded_data,
                        fs_Encoding* used_encoding,
                        PayloadInterface::EncodeFlags* applied_encode_flags,
                        std::shared_ptr<const PayloadInterface> payload) const = 0;

};

} // namespace fsm

#endif //VOC_FRAMEWORK_SIGNALS_CODEC_INTERFACE_H_

/** \}    end of addtogroup */
