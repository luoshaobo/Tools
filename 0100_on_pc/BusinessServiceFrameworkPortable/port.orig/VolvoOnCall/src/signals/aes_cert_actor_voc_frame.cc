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
 *  \file     aes_cert_actor_voc_frame.cc
 *  \brief    VOC Service AES CERT Actor Voc Frame class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/aes_cert_actor_voc_frame.h"

namespace volvo_on_call
{

// ***
// VocFrameEncodableInterface implementation start.
// ***

VocFrameType AesCertActorVocFrame::GetFrameType() const
{
    return VocFrameType::kAesCertActor;
}

// ***
// VocFrameEncodableInterface implementation end.
// ***

#ifdef VOC_TESTS

std::shared_ptr<fsm::Signal> AesCertActorVocFrame::CreateVocFrameSignal(std::vector<unsigned char>& payload)
{
    std::shared_ptr<AesCertActorVocFrame> return_pointer = std::make_shared<AesCertActorVocFrame>();

    if (!return_pointer->SetPayload(payload))
    {
        return_pointer = nullptr;
    }

    return return_pointer;
}

#endif

} // namespace volvo_on_call

/** \}    end of addtogroup */
