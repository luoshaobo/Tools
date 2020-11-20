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
 *  \file     ecdhe_approval_voc_frame.h
 *  \brief    VOC Service AES ECDHE Approval Voc Frame.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_ECDHE_APPROVAL_VOC_FRAME_H_
#define VOC_SIGNALS_AES_ECDHE_APPROVAL_VOC_FRAME_H_

#include "voc_framework/signals/signal.h"

#include <array>
#include <vector>

#include "signals/voc_frame_encodable_interface.h"

namespace volvo_on_call
{

/**
 * \brief Implementation of the ECDHE Approval Voc Frame Type.
 *        This frame type is both encodable and decodable in production.
 */
class EcdheApprovalVocFrame : public fsm::Signal,
                              public VocFrameEncodableInterface
{

 public:

    /**
     * \brief Constructs a new ECDHE Approval frame.
     */
    EcdheApprovalVocFrame();

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    /**
     * \brief Returns the frame type, VocFrameType::kEcdheApprovalVocFrame.
     *
     * \return The frame type, VocFrameType::kEcdheApprovalVocFrame.
     */
    VocFrameType GetFrameType() const;

    /**
     * \brief ECDHE Approval has no payload, this does nothing.
     *
     * \param[in,out] payload Nothing will be done to/with this.
     *
     * \return True.
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

    /**
     * \brief Creates a new EcdheApprovalVocFrame.
     *
     * \param[in] payload Expected to be empty, this frame type has no paylaod.
     *
     * \return a pointer to a EcdheApprovalVocFrame, or nullptr on failure.
     */
    static std::shared_ptr<fsm::Signal> CreateVocFrameSignal(std::vector<unsigned char>& payload);

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_ECDHE_APPROVAL_VOC_FRAME_H_

/** \}    end of addtogroup */
