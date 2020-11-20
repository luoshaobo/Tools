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
 *  \file     aes_csr_voc_frame.h
 *  \brief    VOC Service AES CSR Voc Frame base class.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_AES_CSR_VOC_FRAME_H_
#define VOC_SIGNALS_AES_CSR_VOC_FRAME_H_

#include "voc_framework/signals/signal.h"

#include "signals/voc_frame_encodable_interface.h"

#include <array>
#include <vector>

#include <openssl/x509.h>

namespace volvo_on_call
{

/**
 * \brief Implementation of a generic AES CSR Voc Frame Type.
 */
class AesCsrVocFrame
#ifdef VOC_TESTS
    //Frames are only expected to be decodable in production.
    //encode implementation is test support.
    : public VocFrameEncodableInterface
#endif
{

 public:

    /**
     * \brief Returns the contained signing request.
     *
     * \return The containes Signing request, or nullptr on error.
     */
    X509_REQ* GetRequest();

    /**
     * Destructor. Frees internall CSR.
     */
    ~AesCsrVocFrame();

#ifdef VOC_TESTS

    /**
     * \brief Constructs a AesCsrVocFrame.
     *
     * \param[in] request The certificate signing request
     *                    which the frame should contain.
     *                    Will increase ref count so caller
     *                    should still free its reference to
     *                    the req.
     */
    AesCsrVocFrame(X509_REQ* request);

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
     * \brief Gets contained CSR as der encoded data.
     *
     * \param[out] payload DER data will be appended to this.
     *
     * \return True on success, false on failure.
     */
    bool GetPayload(std::vector<unsigned char>& payload) const;

    // ***
    // VocFrameEncodableInterface implementation end.
    // ***
#endif

 protected:

    /**
     * \brief Constructs an empty AesCsrVocFrame
     */
    AesCsrVocFrame();

    /**
     * \brief Sets contained CSR as der encoded data.
     *
     * \param[in] payload Expected to contain a DER encoded CSR.
     *
     * \return True on success, false on failure.
     */
    bool SetPayload(std::vector<unsigned char>& payload);

 private:

    X509_REQ* request_ = nullptr;

};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_AES_CSR_VOC_FRAME_H_

/** \}    end of addtogroup */
