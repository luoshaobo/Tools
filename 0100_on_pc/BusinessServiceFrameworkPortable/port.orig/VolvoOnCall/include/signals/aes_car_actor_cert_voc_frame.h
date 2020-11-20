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
 *  \file     aes_car_actor_cert_voc_frame.h
 *  \brief    VOC Service AES CERT Car Actor Cert Voc Frame class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_CAR_ACTOR_CERT_VOC_FRAME_H_
#define VOC_SIGNALS_AES_CAR_ACTOR_CERT_VOC_FRAME_H_

#include "signals/aes_cert_voc_frame.h"
#include "signals/signal_types.h"

namespace volvo_on_call
{

/**
 * \brief Implementation of the AES CA Voc Frame Type.
 *        This frame type is only Encodable in production.
 */
class AesCarActorCertVocFrame : public AesCertVocFrame, public fsm::Signal
{

 public:

    /**
     * \brief Constructs a AesCarActorCertVocFrame.
     *
     * \param[in] certificate The certificate signing certificate
     *                        which the frame should contain.
     *                        Will increase ref count so caller
     *                        should still free its reference to
     *                        the req.
     */
    AesCarActorCertVocFrame(X509* certificate) :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        AesCertVocFrame(certificate),
            fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesCarActorCertVocFrame)
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
        return "AesCarActorCertVocFrame";
    }

    // ***
    // Signal implementation end.
    // ***

#ifdef VOC_TESTS

    /**
     * \brief Creates a new AesCarActorCertVocFrame.
     *
     * \param[in] payload Expected to contain a der encoded CERT.
     *
     * \return a pointer to a AesCarActorCertVocFrame, or nullptr on failure.
     */
    static std::shared_ptr<fsm::Signal> CreateVocFrameSignal(std::vector<unsigned char>& payload);

    /**
     * \brief Constructs an empty AesCarActorCertVocFrame
     */
    AesCarActorCertVocFrame() :
        // VocFrames dont have transaction ids but Signals do, we use a "cheap" arbitrary type
        AesCertVocFrame(),
        fsm::Signal(fsm::VehicleCommTransactionId(), VocSignalTypes::kAesCarActorCertVocFrame)
    {};

#endif

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_CAR_ACTOR_CERT_VOC_FRAME_H_

/** \}    end of addtogroup */
