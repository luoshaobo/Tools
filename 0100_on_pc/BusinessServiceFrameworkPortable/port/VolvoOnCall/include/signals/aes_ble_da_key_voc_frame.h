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
 *  \file     aes_ble_da_key_voc_frame.h
 *  \brief    VOC Service AES BL DA KEY Voc Frame.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_BLE_DA_KEY_VOC_FRAME_H_
#define VOC_SIGNALS_AES_BLE_DA_KEY_VOC_FRAME_H_

#include "voc_framework/signals/signal.h"

#include <array>
#include <vector>

#include "signals/voc_frame_encodable_interface.h"

namespace volvo_on_call
{

/**
 * \brief Implements the AES BLE DA KEY Voc Frame which has a 16 byte key as payload.
 */
class AesBleDaKeyVocFrame : public fsm::Signal,
                            public VocFrameEncodableInterface
{

 public:

    /**
     * \brief The size of the BLE Data Access Key.
     */
    static const int kBleDaKeyLength = 16;

    /**
     * \brief Fixed size array holding a BLE Data Access Key.
     */
    typedef std::array<unsigned char, kBleDaKeyLength> BleDaKey;

    /**
     * \brief Constructs a new AesBleDaKeyVocFrame.
     *
     * \param[in] ble_da_key The BLE Data Access key contained in this frame.
     */
    AesBleDaKeyVocFrame(BleDaKey ble_da_key);

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    /**
     * \brief Returns the frame type, VocFrameType::kAesBleDaKey.
     *
     * \return The frame type, VocFrameType::kAesBleDaKey.
     */
    VocFrameType GetFrameType() const;

    /**
     * \brief Get the BLE Data Access Key as a byte sequence.
     *
     * \param[in,out] payload The key will be appended to this.
     *
     * \return True on success, false on failure.
     */
    bool GetPayload(std::vector<unsigned char>& payload) const;

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***


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

#ifdef VOC_TESTS

    // Decode of this frame type not expected in production, test support.

    const BleDaKey& GetKey();

    /**
     * \brief Creates a new AesBleDaKeyVocFrame.
     *
     * \param[in] payload Expected to contain a BDAK.
     *
     * \return a pointer to a AesBleDaKeyVocFrame, or nullptr on failure.
     */
    static std::shared_ptr<fsm::Signal> CreateVocFrameSignal(std::vector<unsigned char>& payload);

#endif
 private:

    /**
     * \brief No default constructor.
     */
    AesBleDaKeyVocFrame();

    /**
     * \brief The contained BLE Data Access Key.
     */
    const BleDaKey ble_da_key_;
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_BLE_DA_KEY_VOC_FRAME_H_

/** \}    end of addtogroup */
