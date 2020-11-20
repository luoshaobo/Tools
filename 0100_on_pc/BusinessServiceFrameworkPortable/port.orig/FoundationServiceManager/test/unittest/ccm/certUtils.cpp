/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     certUtils.cpp
 *  \brief    Utilities for loading OpenSSL Certificates and/or Keys from streams
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

#include <string.h>
#include <stdbool.h>

#include <openssl/err.h>

#include "certUtils.h"


X509* ut_GetCertFromPEM(const char* pPemData)
{
    X509* pCert = NULL;
    BIO* pCertBio = NULL;

    pCertBio = BIO_new(BIO_s_mem());

    if (pCertBio)
    {
        const size_t pemDataLength = strlen(pPemData);
        int writtenDataLength = BIO_write(pCertBio, reinterpret_cast<const void*>(pPemData), pemDataLength);

        // BIO must contain the full data inside the PEM
        if (writtenDataLength == pemDataLength)
        {
            pCert = PEM_read_bio_X509_AUX(pCertBio, NULL, NULL, NULL);
        }
    }

    return pCert;
}

EVP_PKEY* ut_GetKeyFromPEM(const char* pPemData)
{
    EVP_PKEY* pKey = NULL;
    BIO* pKeyBio = NULL;

    pKeyBio = BIO_new(BIO_s_mem());

    if (pKeyBio)
    {
        const size_t pemDataLength = strlen(pPemData);
        int writtenDataLength = BIO_write(pKeyBio, reinterpret_cast<const void*>(pPemData), pemDataLength);

        if (writtenDataLength == pemDataLength)
        {
            pKey = PEM_read_bio_PrivateKey(pKeyBio, NULL, NULL, NULL);
        }
    }

    return pKey;
}

BIO* ut_GetBioFromFile(const char* pPath)
{
    BIO* pBio = NULL;

    if (pPath)
    {
        pBio = BIO_new(BIO_s_file());

        if (pBio)
        {
            if (!BIO_read_filename(pBio, pPath))
            {
                BIO_free(pBio);
                pBio = NULL;
            }
        }
    }

    return pBio;
}

X509* ut_GetCertFromBio(BIO* pBio)
{
    X509* pCert = NULL;

    if (pBio)
    {
        pCert = PEM_read_bio_X509_AUX(pBio, NULL, NULL, NULL);
    }

    return pCert;
}

EVP_PKEY* ut_GetKeyFromBio(BIO* pBio)
{
    EVP_PKEY* pKey = NULL;

    if (pBio)
    {
        pKey = PEM_read_bio_PrivateKey(pBio, NULL, NULL, NULL);
    }

    return pKey;
}

X509_STORE* ut_GetCertStore(X509* pCerts[], size_t certCount)
{
    X509_STORE* pCertStore = NULL;

    pCertStore = X509_STORE_new();
    
    if (pCertStore)
    {
        size_t certIndex = 0;
        bool addedCert = true;

        for (; certIndex < certCount && addedCert; ++certIndex)
        {
            if (!pCerts[certIndex]
                 || !X509_STORE_add_cert(pCertStore, pCerts[certIndex]))
            {
                addedCert = false;
            }
        }

        if (!addedCert)
        {
            // A certificate could not be added.
            // Gracefully fail and return NULL to notify caller on failure.
            X509_STORE_free(pCertStore);
            pCertStore = NULL;
        }
    }

    return pCertStore;
}
/** \}    end of addtogroup */
