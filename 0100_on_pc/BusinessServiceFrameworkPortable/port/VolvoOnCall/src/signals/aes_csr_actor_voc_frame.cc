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
 *  \file     aes_csr_actor_voc_frame.cc
 *  \brief    VOC Service AES CSR Actor Voc Frame class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/aes_csr_actor_voc_frame.h"

#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

#ifdef VOC_TESTS

// ***
// VocFrameEncodableInterface implementation start.
// ***

VocFrameType AesCsrActorVocFrame::GetFrameType() const
{
    return VocFrameType::kAesCsrActor;
}

// ***
// VocFrameEncodableInterface implementation end.
// ***

#endif

std::shared_ptr<fsm::Signal> AesCsrActorVocFrame::CreateVocFrameSignal(std::vector<unsigned char>& payload)
{
    std::shared_ptr<AesCsrActorVocFrame> return_pointer = std::make_shared<AesCsrActorVocFrame>();
    if(return_pointer)              //fix klocwork
    {
        if (!return_pointer->SetPayload(payload))
        {
            return_pointer = nullptr;
        }
    }
    return return_pointer;
}

} // namespace volvo_on_call

/** \}    end of addtogroup */
