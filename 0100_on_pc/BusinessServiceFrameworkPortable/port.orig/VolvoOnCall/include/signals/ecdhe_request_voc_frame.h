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
 *  \file     ecdhe_request_voc_frame.h
 *  \brief    VOC Service AES ECDHE Request Voc Frame.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_ECDHE_REQUEST_VOC_FRAME_H_
#define VOC_SIGNALS_AES_ECDHE_REQUEST_VOC_FRAME_H_

#include <openssl/ec.h>
#include <array>
#include <vector>

#include "voc_framework/signals/signal.h"

#include "signals/voc_frame_encodable_interface.h"


namespace volvo_on_call
{

/**
 * \brief Implementation of the ECDHE Request Voc Frame Type.
 *        This frame type is both encodable and decodable in production.
 */
class EcdheRequestVocFrame : public fsm::Signal,
                             public VocFrameEncodableInterface
{

 public:

    /**
     * \brief Constructs a new ECDHE Request frame.
     *
     * \param[in] key evp_pkey.
     *            Caller must still free the pointer.
     */
    EcdheRequestVocFrame(EVP_PKEY* key);

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    /**
     * \brief Returns the frame type, VocFrameType::kEcdheRequestVocFrame.
     *
     * \return The frame type, VocFrameType::kEcdheRequestVocFrame.
     */
    VocFrameType GetFrameType() const;

    /**
     * \brief Gets the EC key as a octet string
     *
     * \param[out] payload The EC key will appended to the payload.
     *
     * \return True on success, false on failure.
     */
    bool GetPayload(std::vector<unsigned char>& payload) const;

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***

    /**
     * \brief Returns the EC Key as a EVP_PKEY.
     *
     * \return A EVP_PKEY, or nullptr on failure.
     *         Caller must free the key.
     */
    EVP_PKEY* GetKey();


    // ***
    // Signal implementation start.
    // ***

    /**
     * \brief Returns the name of the signal.
     *
     * \return The name of the signal.
     */
    std::string ToString();

    // ***
    // Signal implementation end.
    // ***

    /**
     * \brief Creates a new EcdheRequestVocFrame.
     *
     * \param[in] payload Should contain an EC key as octet string.
     *
     * \return Pointer to a EcdheRequestVocFrame, or nullptr on failure.
     */
    static std::shared_ptr<fsm::Signal> CreateVocFrameSignal(std::vector<unsigned char>& payload);

 private:

    EVP_PKEY* key_;
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_ECDHE_REQUEST_VOC_FRAME_H_

/** \}    end of addtogroup */
