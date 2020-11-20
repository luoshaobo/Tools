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
 *  \file     voc_frame_types.h
 *  \brief    VOC Service Voc Frame types.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#ifndef VOC_SIGNALS_VOC_FRAME_TYPES_H_
#define VOC_SIGNALS_VOC_FRAME_TYPES_H_

namespace volvo_on_call
{

/**
 * \brief Voc frame identifier, first byte in header.
 */
static const unsigned char kVocFrameIdentifier = 0xFE;

/**
 * \brief Size of the voc frame header, in bytes.
 */
static const int kVocFrameHeaderSize = 4;

/**
 * \brief Enumeration of known frame types.
 */
enum VocFrameType : unsigned char
{
    kUndefined = 0, //*< default undefined value
    kEcdheRequest = 0x01, //*< ECDHE request, contains public key as octet string
    kEcdheApproval = 0x02, //*< ECDHE approval, no payload
    kAes = 0x03, //*< Generic AES frame
    kAesCsrActor = 0x04, //*< Encrypted Actor CSR, payload is DER encoded CSR.
    kAesCsrNearfield = 0x05, //*< Encrypted Nearfield TLS client CSR, payload is DER encoded CSR.
    kAesCertActor = 0x06, //*< Encrypted Actor Certificate, payload is DER encoded certificate.
    kAesCertNearfield = 0x07, //*< Encrypted Nearfield TLS client certificate, payload is DER encoded certificate.
    kAesCa = 0x08, //*< Encrypted Vehicle CA, payload is DER encoded certificate.
    kTls = 0x09, //*< Generic TLS frame.
    kAesBleDaKey = 0x0A, //*< Encrypted BLE data access key, payload is 16 byte key.
    kAesCarActorCert = 0x0B //*< Encrypted Vehicle Actor certificate, payload is DER encoded certificate.
};

} // namespace volvo_on_call

#endif //VOC_SIGNALS_VOC_FRAME_TYPES_H_

/** \}    end of addtogroup */
