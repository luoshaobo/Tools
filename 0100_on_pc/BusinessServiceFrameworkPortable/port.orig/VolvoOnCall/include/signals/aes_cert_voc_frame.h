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
 *  \file     aes_cert_voc_frame.h
 *  \brief    VOC Service AES CERT Voc Frame base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_CERT_VOC_FRAME_H_
#define VOC_SIGNALS_AES_CERT_VOC_FRAME_H_

#include "voc_framework/signals/signal.h"

#include "signals/voc_frame_encodable_interface.h"

#include <array>
#include <vector>

#include <openssl/x509.h>

namespace volvo_on_call
{

/**
 * \brief Implementation of a generic AES CERT Voc Frame Type.
 */
class AesCertVocFrame : public VocFrameEncodableInterface
{

 public:

    /**
     * \brief Constructs a AesCertVocFrame.
     *
     * \param[in] certificate The certificate signing certificate
     *                        which the frame should contain.
     *                        Will increase ref count so caller
     *                        should still free its reference to
     *                        the req.
     */
    AesCertVocFrame(X509* certificate);

    /**
     * Destructor. Frees internal CERT.
     */
    ~AesCertVocFrame();

    // ***
    // VocFrameEncodableInterface implementation start.
    // ***

    /**
     * \brief Returns the frame type. Subclasses must implement.
     *
     * \return The frame type.
     */
    virtual VocFrameType GetFrameType() const = 0;

    /**
     * \brief Gets contained CERT as der encoded data.
     *
     * \param[out] payload DER data will be appended to this.
     *
     * \return True on success, false on failure.
     */
    bool GetPayload(std::vector<unsigned char>& payload) const;

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***

#ifdef VOC_TESTS

    //Encode support is under only for test support.

    /**
     * \brief Returns the contained certificate.
     *
     * \return The containes Signing certificate, or nullptr on error.
     */
    X509* GetCertificate();

 protected:

    /**
     * \brief Constructs an empty AesCertVocFrame
     */
    AesCertVocFrame();

    /**
     * \brief Sets contained CERT as der encoded data.
     *
     * \param[in] payload Expected to contain a DER encoded CERT.
     *
     * \return True on success, false on failure.
     */
    bool SetPayload(std::vector<unsigned char>& payload);

#endif

 private:

    X509* certificate_ = nullptr;

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_CERT_VOC_FRAME_H_

/** \}    end of addtogroup */
