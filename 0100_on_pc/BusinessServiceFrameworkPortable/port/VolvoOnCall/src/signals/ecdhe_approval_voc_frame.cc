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
 *  \file     ecdhe_approval_voc_frame.cc
 *  \brief    VOC Service AES BL DA KEY Voc Frame.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/ecdhe_approval_voc_frame.h"
#include "signals/signal_types.h"

#include <algorithm>

namespace volvo_on_call
{

    EcdheApprovalVocFrame::EcdheApprovalVocFrame() :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kEcdheApprovalVocFrame)
    {}

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    VocFrameType EcdheApprovalVocFrame::GetFrameType() const
    {
        return kEcdheApproval;
    }

    bool EcdheApprovalVocFrame::GetPayload(std::vector<unsigned char>& payload) const
    {
        return true;
    }

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***

    // ***
    // Signal implementation start.
    // ***

    std::string EcdheApprovalVocFrame::ToString()
    {
        return "EcdheApprovalVocFrame";
    }

    // ***
    // Signal implementation end.
    // ***

    std::shared_ptr<fsm::Signal> EcdheApprovalVocFrame::CreateVocFrameSignal(std::vector<unsigned char>& payload)
    {
        return std::make_shared<EcdheApprovalVocFrame>();
    }

} // namespace volvo_on_call

/** \}    end of addtogroup */
