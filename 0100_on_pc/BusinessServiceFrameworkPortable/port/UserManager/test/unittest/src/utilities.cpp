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
 *  \file     utilities.cpp
 *  \brief    Unit-Test utilities that do not fall under any category.
 *  \author   Iulian Sirghi
 *
 *  \addtogroup unittest
 *  \{
 */

#include "utilities.h"

#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include <cstdio>
#include <fstream>
#include <streambuf>


#include <iostream>
bool FileExists(const char* path)
{
    FILE* file = fopen(path, "r");

    if (!file)
        return false;

    fclose(file);

    return true;
}

void GeneratePrivateKey(EVP_PKEY** key, int key_length)
{
    RSA* rsa;

    *key = EVP_PKEY_new();
    rsa = RSA_generate_key(key_length, RSA_F4, NULL, NULL);
    EVP_PKEY_assign_RSA(*key, rsa);

}

void GenerateCertificate(X509** cert, EVP_PKEY* key, std::string cn)
{
    static int counter_for_unique_cn = 0;
    std::string unique_cn = "localhost" + std::to_string(counter_for_unique_cn++);
    *cert = X509_new();
    ASN1_INTEGER_set(X509_get_serialNumber(*cert), 1);
    X509_gmtime_adj(X509_get_notBefore(*cert), 0);
    X509_gmtime_adj(X509_get_notAfter(*cert), 3600);

    X509_set_pubkey(*cert, key);

    X509_NAME* name;
    name = X509_get_subject_name(*cert);

    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char*)"CA", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"Test Inc.", -1, -1, 0);
    if (cn.empty())
    {
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                                   (unsigned char *)(unique_cn.c_str()), -1, -1, 0);
    }
    else
    {
                X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                                   (unsigned char *)(cn.c_str()), -1, -1, 0);
    }

    X509_set_issuer_name(*cert, name);


    // Add SKI
    X509_EXTENSION *ex;
    X509V3_CTX ctx;

    // Empty configuration database that needs to be added to our context,
    // otherwise X509V3_EXT_conf_nid fails with X509V3_R_NO_CONFIG_DATABASE error
    // when adding the NID_certificate_policies extension
    CONF conf = {nullptr, nullptr, nullptr};

    /*
     * Issuer and subject certs: both the target since it is self signed, no
     * request and no CRL
     */
    X509V3_set_ctx(&ctx, *cert, *cert, NULL, NULL, 0);
    X509V3_set_nconf(&ctx, &conf);

    ex = X509V3_EXT_conf_nid(NULL, &ctx, NID_subject_key_identifier, (char*) "hash");

    if (!ex)
    {
        std::abort();
    }
    else
    {
        X509_add_ext(*cert, ex, -1);

        X509_EXTENSION_free(ex);
    }

    X509_sign(*cert, key, EVP_sha1());
}

std::string CertToPem(X509* cert)
{
    // First create PEM buffer of certificate
    BIO* reference_bio = BIO_new(BIO_s_mem());
    int num_reference_bytes = 0;
    void* reference_data = nullptr;

    PEM_write_bio_X509(reference_bio, cert);
    num_reference_bytes = BIO_get_mem_data(reference_bio, &reference_data);

    std::string pem_data;

    pem_data.append(reinterpret_cast<char*>(reference_data), num_reference_bytes);

    BIO_free(reference_bio);

    return pem_data;
}

std::string KeyToPem(EVP_PKEY* key)
{
    // First create PEM buffer of certificate
    BIO* reference_bio = BIO_new(BIO_s_mem());
    int num_reference_bytes = 0;
    void* reference_data = nullptr;

    PEM_write_bio_PKCS8PrivateKey(reference_bio, key, nullptr, nullptr, 0, 0, nullptr);
    num_reference_bytes = BIO_get_mem_data(reference_bio, &reference_data);

    std::string pem_data;
    pem_data.append(reinterpret_cast<char*>(reference_data), num_reference_bytes);

    BIO_free(reference_bio);

    return pem_data;
}

std::string FileToPem(std::string path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string return_value((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    return return_value;
}

void GetCommonName(std::vector<unsigned char>& cn, X509* certificate)
{
    unsigned char* common_name = nullptr;
    int common_name_length = 0;

    X509_NAME* name = nullptr;
    X509_NAME_ENTRY* name_entry = nullptr;
    int last_index = -1;

    name = X509_get_subject_name(certificate);

    if (name)
    {
        last_index = X509_NAME_get_index_by_NID(name, NID_commonName, last_index);

        if (last_index != -1)
        {
            name_entry = X509_NAME_get_entry(name, last_index);

            if (name_entry)
            {
                ASN1_STRING* common_name_data =  X509_NAME_ENTRY_get_data(name_entry);
                if (common_name_data)
                {
                    // we get the raw data rather than treating it as text
                    // as we expect UUIDs
                    common_name = ASN1_STRING_data(common_name_data);
                    common_name_length = ASN1_STRING_length(common_name_data);
                }
            }
        }
    }

    if (common_name)
    {
        cn.assign(common_name, common_name + common_name_length);
    }
}

void DumpMessageFile(std::string name, const char* data, size_t data_size)
{
    #ifdef SAVE_SIGNAL_FILES
    if(nullptr == data)
        return;

    std::ofstream file(name.c_str(), std::ios::out | std::ios::binary);
    if (file.is_open())
        {
            file.write(data, data_size);
            file.close();
        }
    #endif
}

/** \}    end of addtogroup */
