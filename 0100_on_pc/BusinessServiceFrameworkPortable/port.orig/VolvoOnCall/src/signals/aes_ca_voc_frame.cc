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
 *  \file     aes_ca_voc_frame.cc
 *  \brief    VOC Service AES CA Voc Frame class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/aes_ca_voc_frame.h"

namespace volvo_on_call
{

// ***
// VocFrameEncodableInterface implementation start.
// ***

VocFrameType AesCaVocFrame::GetFrameType() const
{
    return VocFrameType::kAesCa;
}

// ***
// VocFrameEncodableInterface implementation end.
// ***

#ifdef VOC_TESTS

std::shared_ptr<fsm::Signal> AesCaVocFrame::CreateVocFrameSignal(std::vector<unsigned char>& payload)
{
    std::shared_ptr<AesCaVocFrame> return_pointer = std::make_shared<AesCaVocFrame>();

    if (!return_pointer->SetPayload(payload))
    {
        return_pointer = nullptr;
    }

    return return_pointer;
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
