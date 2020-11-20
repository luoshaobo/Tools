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
 *  \file     aes_cert_nearfield_voc_frame.h
 *  \brief    VOC Service AES CERT Nearfield Voc Frame class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_CERT_NEARFIELD_VOC_FRAME_H_
#define VOC_SIGNALS_AES_CERT_NEARFIELD_VOC_FRAME_H_

#include "signals/aes_cert_voc_frame.h"
#include "signals/signal_types.h"

namespace volvo_on_call
{

/**
 * \brief Implementation of the AES CERT Nearfield Voc Frame Type.
 *        This frame type is only encodable in production.
 */
class AesCertNearfieldVocFrame : public AesCertVocFrame, public fsm::Signal
{

 public:

    /**
     * \brief Constructs a AesCertNearfieldVocFrame.
     *
     * \param[in] certificate The certificate signing certificate
     *                        which the frame should contain.
     *                        Will increase ref count so caller
     *                        should still free its reference to
     *                        the req.
     */
    AesCertNearfieldVocFrame(X509* certificate) :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        AesCertVocFrame(certificate),
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesCertNearfieldVocFrame)
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

    // ***
    // Signal implementation start.
    // ***

    std::string ToString()
    {
        return "AesCertNearfieldVocFrame";
    }

    // ***
    // Signal implementation end.
    // ***

#ifdef VOC_TESTS

    /**
     * \brief Creates a new AesCertNearfieldVocFrame.
     *
     * \param[in] payload Expected to contain a der encoded CERT.
     *
     * \return a pointer to a AesCertNearfieldVocFrame, or nullptr on failure.
     */
    static std::shared_ptr<fsm::Signal> CreateVocFrameSignal(std::vector<unsigned char>& payload);

    /**
     * \brief Constructs an empty AesCertNearfieldVocFrame
     */
    AesCertNearfieldVocFrame() :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        AesCertVocFrame(),
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesCertNearfieldVocFrame)
    {};

#endif

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_CERT_NEARFIELD_VOC_FRAME_H_

/** \}    end of addtogroup */
