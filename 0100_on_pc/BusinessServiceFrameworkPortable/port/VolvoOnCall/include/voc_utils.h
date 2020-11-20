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
 *  file     vox_utils.h
 *  brief    VoC related utility routines.
 *  author   Maksym Mozok, et al.
 *
 *  addtogroup VolvoOnCall
 *  {
 */
#ifndef VOC_UTILS_H
#define VOC_UTILS_H

#include "voc_framework/transactions/transaction_id.h"

#include <atomic>
#include <mutex>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>


namespace volvo_on_call
{
/**
 * \brief Seed size to use with random number generator.
 */
const int kRndGenSeedSize = 32;
// amount of time to wait for libfssd_Init in milliseconds
const int kLibFssdInitTimout = 30;
// number of attmepts that shOBould be done to init libfssd before giving up
const int kLibFssdInitMaxAttempts = 10;


class ThreadSafeReferenceCounter
{
    std::atomic_int_fast16_t reference_counter_;

    public:
        ThreadSafeReferenceCounter();
        void IncrUse(void);
        void DecrUse(void);
        int_fast16_t Get_count(void){return reference_counter_.load(std::memory_order_relaxed);}
};

/**
 * \brief Global init function. Add any additional global init here.
*/
void InitVocCustom(void);

/**
 * \brief Global cleanup function. Add any additional global cleanup here.
 *
 */
void CleanupVocCustom(void);

/**
 * \brief Generate a serial number (16 most significant bytes of X509's SKI) and write it to cert.
 *
 * \param[in] cert X509 certificate the generated serial number will be written to.
 *
 * \return True in case of success false in case of error.
 */
bool SetSerialNumber(X509 *cert);
/**
 * \brief Creates and adds X509_EXTENSION
 *
 * \param[in] issuer Issuer of the subject certificate.
 *
 * \param[in] subject The target certificate to get the extension.
 *
 * \param[in] nid Nid of X509 extension (see openssl/crypto/objects/obj_mac.h).
 *
 * \param[in] value String containing the extension value.
 *
 * \return True in case of success, false otherwise.
 */
bool AddExtension(X509 *issuer, X509 *subject, int nid, char *value);


/**
 * \brief Signs the signing_request with vehicle CA keys, sets the validity period to 730 days (from now on)
 *  and sets the serial number (according to specs) and the issuer. Does not perform any other checks.
 *
 * \param[in]  signing_request  the signing request in X509_REQ format; don't assume it will be untouched
 * \param[out] signed_certificate   signed certificate with vehicle's CA key. Provide empty pointer. Returned pointer must be freed by caller
 *
 * \return true in case of success, false in case of any failure
 */
bool SignCert(X509_REQ *signing_request, X509 **signed_certificate);

} // namespace volvo_on_call
#endif // VOC_UTILS_H


/** \}    end of addtogroup */
