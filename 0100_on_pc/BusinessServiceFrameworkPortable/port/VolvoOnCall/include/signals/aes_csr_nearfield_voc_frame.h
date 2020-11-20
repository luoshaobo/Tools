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
 *  \file     aes_csr_nearfield_voc_frame.h
 *  \brief    VOC Service AES CSR Nearfield Voc Frame class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_CSR_NEARFIELD_VOC_FRAME_H_
#define VOC_SIGNALS_AES_CSR_NEARFIELD_VOC_FRAME_H_

#include "signals/aes_csr_voc_frame.h"
#include "signals/signal_types.h"

namespace volvo_on_call
{

/**
 * \brief Implementation of the AES CSR Nearfield Voc Frame Type.
 *        This frame type is only decodable in production.
 */
class AesCsrNearfieldVocFrame : public AesCsrVocFrame, public fsm::Signal
{

 public:

#ifdef VOC_TESTS

    /**
     * \brief Constructs a AesCsrNearfieldVocFrame.
     *
     * \param[in] request The certificate signing request
     *                    which the frame should contain.
     *                    Will increase ref count so caller
     *                    should still free its reference to
     *                    the req.
     */
    AesCsrNearfieldVocFrame(X509_REQ* request) :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        AesCsrVocFrame(request),
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesCsrNearfieldVocFrame)
    {};

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    /**
     * \brief Returns the frame type.
     *
     * \return The frame type.
     */
    VocFrameType GetFrameType() const;

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***
#endif

    // ***
    // Signal implementation start.
    // ***

    std::string ToString()
    {
        return "AesCsrNearfieldVocFrame";
    }

    // ***
    // Signal implementation end.
    // ***

    /**
     * \brief Creates a new AesCsrNearfieldVocFrame.
     *
     * \param[in] payload Expected to contain a der encoded CSR.
     *
     * \return a pointer to a AesCsrNearfieldVocFrame, or nullptr on failure.
     */
    static std::shared_ptr<fsm::Signal> CreateVocFrameSignal(std::vector<unsigned char>& payload);

    /**
     * \brief Constructs an empty AesCsrNearfieldVocFrame
     */
    AesCsrNearfieldVocFrame() :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        AesCsrVocFrame(),
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesCsrNearfieldVocFrame)
    {};

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_CSR_NEARFIELD_VOC_FRAME_H_

/** \}    end of addtogroup */
