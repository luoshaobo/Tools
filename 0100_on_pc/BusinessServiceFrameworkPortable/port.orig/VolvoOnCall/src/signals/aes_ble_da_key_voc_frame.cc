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
 *  \file     aes_ble_da_key_voc_frame.cc
 *  \brief    VOC Service AES BL DA KEY Voc Frame.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/aes_ble_da_key_voc_frame.h"
#include "signals/signal_types.h"


#include <algorithm>

namespace volvo_on_call
{

    AesBleDaKeyVocFrame::AesBleDaKeyVocFrame(BleDaKey ble_da_key) :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesBleDaKeyVocFrame),
        ble_da_key_(ble_da_key)
    {}

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    VocFrameType AesBleDaKeyVocFrame::GetFrameType() const
    {
        return kAesBleDaKey;
    }

    bool AesBleDaKeyVocFrame::GetPayload(std::vector<unsigned char>& payload) const
    {
        std::copy_n(ble_da_key_.begin(), kBleDaKeyLength, std::back_inserter(payload));

        return true;
    }

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***

    // ***
    // Signal implementation start.
    // ***

    std::string AesBleDaKeyVocFrame::ToString()
    {
        return "AesBleDaKeyVocFrame";
    }

    // ***
    // Signal implementation end.
    // ***

#ifdef VOC_TESTS

    const AesBleDaKeyVocFrame::BleDaKey& AesBleDaKeyVocFrame::GetKey()
    {
        return ble_da_key_;
    }

    std::shared_ptr<fsm::Signal> AesBleDaKeyVocFrame::CreateVocFrameSignal(std::vector<unsigned char>& payload)
    {
        std::shared_ptr<fsm::Signal> return_signal = nullptr;

        // check payload is of expected length
        // as key is of static size
        if (payload.size() == kBleDaKeyLength)
        {
            BleDaKey ble_da_key;
            std::copy_n(payload.begin(), kBleDaKeyLength, ble_da_key.begin());

            return_signal = std::make_shared<AesBleDaKeyVocFrame>(ble_da_key);
        }

        return return_signal;
    }

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
