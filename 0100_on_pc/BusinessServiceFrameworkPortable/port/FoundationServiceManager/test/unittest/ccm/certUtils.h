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
 *  \file     certUtils.h
 *  \brief    Utilities for loading OpenSSL Certificates and/or Keys from streams
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

#ifndef C_UNITTEST_CCM_CERTUTILS_H_
#define C_UNITTEST_CCM_CERTUTILS_H_

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

/**
 * \brief ut_GetCertFromPEM loads X509 certificate from PEM-formatted string.
 * Converts base64 DER-encoded PEM string to OpenSSL X509 certificate structure.
 * Caller is responsible to free object via X509_free if returned pointer is non-null.
 *
 * \param [in] pPemData PEM-formatted string.
 *
 * \return null-pointer if pPemData is null or malformed, valid pointer otherwise.
 */
X509* ut_GetCertFromPEM(const char* pPemData);

/**
 * \brief ut_GetKeyFromPEM loads OpenSSL cryptography key from PEM-formatted string.
 * Converts base64 DER-encoded PEM string to OpenSSL EVP_PKEY structure.
 * Caller is responsible to free object via EVP_PKEY_free if returned pointer is non-null.
 *
 * \param [in] pPemData PEM-formatted string.
 *
 * \return null-pointer if pPemData is null or malformed, valid pointer otherwise.
 */
EVP_PKEY* ut_GetKeyFromPEM(const char* pPemData);

/**
 * \brief ut_GetBioFromFile instatiate OpenSSL stream object referencing file-system record.
 * Caller is responsible to free object via BIO_free if returned pointer is non-null.
 *
 * \param [in] pPath path to file
 *
 * \return null-pointer if pPath is null or if it references non-readable/malformed record, valid pointer otherwise.
 */
BIO* ut_GetBioFromFile(const char* pPath);

/**
 * \brief ut_GetCertFromBio loads OpenSSL X509 certificate from stream object.
 * Caller is responsible to free object via X509_free if returned pointer is non-null.
 *
 * \param [in] pBio pointer to readable stream object
 *
 * \return null-pointer if stream object is null or not redable, valid pointer otherwise.
 */
X509* ut_GetCertFromBio(BIO* pBio);

/**
 * \brief ut_GetKeyFromBio loads OpenSSL cryptography key from stream object.
 * Caller is responsible to free object via EVP_PKEY_free if returned pointer is non-null.
 *
 * \param [in] pBio pointer to readable stream object
 *
 * \return null-pointer if stream object is null or if it's non-readable/malformed, valid pointer otherwise.
 */
EVP_PKEY* ut_GetKeyFromBio(BIO* pBio);

/**
 * \brief ut_GetKeyFromBio loads OpenSSL certificate store from array of certificate objects.
 * Caller is responsible to free object via X509_STORE_free if returned pointer is non-null.
 *
 * \param [in] pCerts array of certificate objects
 * \param [in] certCount number of certificate objects in array
 *
 * \return null-pointer if certificate array is null or cannot be populated, valid pointer otherwise.
 */
X509_STORE* ut_GetCertStore(X509* pCerts[], size_t certCount);

#endif /* C_CCM_CERTUTILS_H_ */
/** \}    end of addtogroup */
