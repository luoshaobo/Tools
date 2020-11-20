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
 *  \file     utilities.h
 *  \brief    Unit-Test utilities that do not fall under any category.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <openssl/x509.h>
#include <openssl/evp.h>

#include <string>
#include <vector>

bool FileExists(const char* path);

// Will generate a self-signed X509 certificate.
// Caller needs to free with X509_free
void GenerateCertificate(X509** cert, EVP_PKEY* key, std::string cn = std::string());

// Will generate a RSA private key with given key_length.
// Caller needs to free with EVP_PKEY_free.
void GeneratePrivateKey(EVP_PKEY** key, int key_length);

// Will convert a X509 cert to it's PEM representation.
std::string CertToPem(X509* cert);

// Will convert an EVP_PKEY cert to it's PEM representation.
std::string KeyToPem(EVP_PKEY* cert);

// Will convert a cert/key file to it's PEM representation.
std::string FileToPem(std::string cert);

// Get coommon name from cert
void GetCommonName(std::vector<unsigned char>& cn, X509* certificate);

// Dump data to file
void DumpMessageFile(std::string name, const char* data, size_t data_size);

#endif // UTILITIES_H

/** \}    end of addtogroup */
