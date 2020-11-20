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
 *  file     voc_utils.cc
 *  brief    voc related utils like custom inits and cleanups, etc.
 *  author   Maksym Mozok
 *
 *  addtogroup VolvoOnCall
 *  {
 */
#include "dlt/dlt.h"

#include "fsm.h"
#include "keystore.h"

#ifndef VOC_TESTS
extern "C"
{
#include "tpsys.h"
}
#endif

#include "voc_utils.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

ThreadSafeReferenceCounter execution_guardian;
std::mutex init_guard;
std::mutex cleanup_guard;

ThreadSafeReferenceCounter::ThreadSafeReferenceCounter():reference_counter_(0)
{
}

void ThreadSafeReferenceCounter::IncrUse(void)
{
    ++reference_counter_;
}
void ThreadSafeReferenceCounter::DecrUse(void)
{
    --reference_counter_;
}

void InitVocCustom(void)
{
    std::lock_guard<std::mutex> lock(volvo_on_call::init_guard);

    if(!volvo_on_call::execution_guardian.Get_count())
    {
        OpenSSL_add_all_algorithms();
    }
    volvo_on_call::execution_guardian.IncrUse();
    // Seed the openssl PRNG
    //TODO: switch to /dev/random whenever its fixed
    if (RAND_load_file("/dev/urandom", kRndGenSeedSize) != kRndGenSeedSize)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to seed PRNG.");
        exit(EXIT_FAILURE);
    }

#ifndef VOC_TESTS
    // Must initialize tpSYS IPC first as libs may depend on it.
    if (tpSYS_initIPC(E_SYS_IPC_DBUS, NULL, NULL) != E_SYS_IPC_RET_SUCCESS)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to intitialize tpSYS IPC.");
        exit(EXIT_FAILURE);
    }

    // Initialize libfsm
    FsmInitialize();
    DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP :  provideIPcommunication : OK");
#endif
}

void CleanupVocCustom(void)
{
    std::lock_guard<std::mutex> lock(volvo_on_call::cleanup_guard);

    // tear down the libs
#ifndef VOC_TESTS
    FsmTerminate();
#endif

    volvo_on_call::execution_guardian.DecrUse();

    if(!volvo_on_call::execution_guardian.Get_count())
    {
        EVP_cleanup();
        // And finally down tpsys services
#ifndef VOC_TESTS
        tpSYS_deinitIPC();
#endif
    }
}

// some utility functions:

bool SetSerialNumber(X509 *cert)
{
    bool result = true;
    ASN1_OCTET_STRING *skid;

    if (result)
    {
        skid = (ASN1_OCTET_STRING *)X509_get_ext_d2i(cert, NID_subject_key_identifier, NULL, NULL);

        if (!skid)
        {
            result = false;
        }
    }

    // Set serial number (16 most siginficant bytes of SKI)
    if (result)
    {
        const int serial_num_len = 16;
        const int serial_num_start_pos_in_SKI = skid->length - serial_num_len;
        BIGNUM *bn = NULL;

        if (result)
        {
            bn = BN_bin2bn(&skid->data[serial_num_start_pos_in_SKI], serial_num_len, NULL);

            if (!bn)
            {
                result = false;
            }
        }

        ASN1_INTEGER *asn1_int = NULL;

        if (result)
        {
            asn1_int = BN_to_ASN1_INTEGER(bn, NULL);

            if (!asn1_int)
            {
                result = false;
            }
        }

        if (result && !X509_set_serialNumber(cert, asn1_int))
        {
            result = false;
        }

        if (asn1_int)
        {
            ASN1_INTEGER_free(asn1_int);
        }

        if (bn)
        {
            BN_free(bn);
        }
    }

    if (skid)
    {
        ASN1_OCTET_STRING_free(skid);
    }

    return result;
}


bool AddExtension(X509 *issuer, X509 *subject, int nid, char *value)
{
    bool result = true;
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
    X509V3_set_ctx(&ctx, issuer, subject, NULL, NULL, 0);
    X509V3_set_nconf(&ctx, &conf);

    ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);

    if (!ex)
    {
        result = false;
    }
    else
    {
        X509_add_ext(subject, ex, -1);

        X509_EXTENSION_free(ex);
    }

    return result;
}


bool SignCert(X509_REQ *signing_request, X509 **signed_certificate)
{
    bool result = true;  //!< overall result of this function, true means success, false means error
    X509* new_cert = nullptr;  //!< working data, which will become the result in case of success (signed_certificate)
    EVP_PKEY* signer_key = nullptr;  //!< signer key, taken from keystore
    X509 *signer_certificate = nullptr; //!< signer certificate, taken from keystore
    EVP_PKEY* signing_request_key = nullptr;  //!< extracted key of signing_request
    int signature_valid_days = (2*365);  //!< validity of certificate. Maximum two years (not considering leap years :)
    const EVP_MD* signer_message_digest = EVP_sha256();  //!< signing digest algorithm

    // check whether provided signing_request is valid:
/* find a better way to check this with new OpenSSL 1.1.0
    result = result && (X509_REQ_get_pubkey() != nullptr)
                    && (&signing_request->req_info != nullptr)
                    && (signing_request->req_info->pubkey != nullptr)
                    && (signing_request->req_info->pubkey->public_key != nullptr)
                    && (signing_request->req_info->pubkey->public_key->data != nullptr);
*/
    result = result && (X509_REQ_get_pubkey(signing_request) != nullptr);
    if (! result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: provided signing_request is invalid");
    }

    // get CA key and certificate from keystore:
    if (result)
    {
        fsm::Keystore keystore;
        fsm::ReturnCode keystore_return_code;
        keystore_return_code = keystore.GetKey(&signer_key, fsm::Keystore::KeyRole::kVehicleCa);
        result = result && (keystore_return_code == fsm::ReturnCode::kSuccess);

        STACK_OF(X509) *certificates = sk_X509_new_null();
        keystore_return_code = keystore.GetCertificates(certificates,
                                                        fsm::Keystore::CertificateRole::kVehicleCa);
        result = result && (keystore_return_code == fsm::ReturnCode::kSuccess);
        if (result)
        {
            if (sk_X509_num(certificates) != 1)
            {
                result = false;
            }
            else
            {
                signer_certificate = sk_X509_pop(certificates);
            }
        }
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: reading from keystore failed");
        }
    }

    // extract public key from signing request:
    if (result)
    {
        signing_request_key = X509_REQ_get_pubkey(signing_request);
        result = result && (signing_request_key != nullptr);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_REQ_get_pubkey failed");
        }
    }

    // convert signing_request to X509* new_cert certificate:
    if (result)
    {
        new_cert = X509_new();
        result = result && (new_cert != nullptr);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_new failed");
        }
    }

    // copy subject name from signing_request:
    if (result)
    {
        int int_result = X509_set_subject_name(new_cert, X509_REQ_get_subject_name(signing_request));
        result = result && (int_result == 1);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_set_subject_name failed");
        }
    }

    //TODO: possible workaround for missing fields in subject

    // Get pointer to x509's name struct (don't free it as it's internal)
    X509_NAME *name = X509_get_subject_name(new_cert);

    if (!name)
    {
        result = false;
    }

    // Set the DC
    if (result)
    {
        unsigned char *domain_component = (unsigned char*)"Volvo Cars";

        if (!X509_NAME_add_entry_by_txt(name, "DC", MBSTRING_ASC, domain_component, -1, -1, 0))
        {
            result = false;
        }
        else
        {
            domain_component = (unsigned char*)"Car Data Signing";

            if (!X509_NAME_add_entry_by_txt(name, "DC", MBSTRING_ASC, domain_component, -1, -1, 0))
            {
                result = false;
            }
        }
    }

    // Set the UID
    if (result)
    {
        unsigned char *user_identifier = (unsigned char*)"MAPP";

        if (!X509_NAME_add_entry_by_txt(name, "UID", MBSTRING_ASC, user_identifier, -1, -1, 0))
        {
            result = false;
        }
    }

    // Set subject name
    if (result)
    {
        if (!X509_set_subject_name(new_cert, name))
        {
            result = false;
        }
    }

    // END TODO

    // copy public key
    if (result)
    {
        int int_result = X509_set_pubkey(new_cert, signing_request_key);
        result = result && (int_result > 0);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_set_pubkey failed");
        }
    }

    // set issuer name in certificate to be signed:
    if (result)
    {
        int int_result = X509_set_issuer_name(new_cert, X509_get_subject_name(signer_certificate));
        result = result && (int_result == 1);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_set_issuer_name failed");
        }
    }

    // set subjct key identifier (ski):
    if (result)
    {
        bool bool_result = AddExtension(new_cert, new_cert, NID_subject_key_identifier, (char*) "hash" );
            // "hash" calculates the hash of private key as specified in the requirements.
        result = result && bool_result;
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: AddExtension failed");
        }
    }

    // set serial number
    if (result)
    {
        bool bool_result = SetSerialNumber(new_cert);
        result = result && bool_result;
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: SetSerialNumber failed");
        }
    }

    // Set the key usage extension
    if (result)
    {
        if (!AddExtension(new_cert, new_cert, NID_key_usage, (char *)"critical,digitalSignature,keyEncipherment"))
        {
            result = false;
        }
    }

    // Set the policy OID extension
    if (result)
    {
        if (!AddExtension(new_cert, new_cert, NID_certificate_policies, (char *)"1.3.6.1.4.1.37916.5.1.3"))
        {
            result = false;
        }
    }

    // set certificate validity from now on:
    if (result)
    {
        ASN1_TIME* asn1_time_ret = X509_time_adj_ex(X509_get_notBefore(new_cert), 0, 0L, NULL);
            // should be X509_get0_notBefore, because X509_get_notBefore is deprecated, but not availabe in our version
        result = result && (asn1_time_ret != NULL);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_time_adj_ex (1) failed");
        }
    }

    // set end of validity in days from now on:
    if (result)
    {
        ASN1_TIME* asn1_time_ret = X509_time_adj_ex(X509_get_notAfter(new_cert), signature_valid_days, 0L, NULL);
            // should be X509_get0_notAfter, because X509_get_notAfter is deprecated, but not availabe in our version
        result = result && (asn1_time_ret != NULL);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_time_adj_ex (2) failed");
        }
    }

    // Set certificate version
    if (!X509_set_version(new_cert, 2)) //v3
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s Failed to set certificate version",
                        __FUNCTION__);
         result = false;
    }

    // Sign the certificate
    if (result)
    {
        int int_result = X509_sign(new_cert, signer_key, signer_message_digest);
        result = result && (int_result != 0);
        if (! result)
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "SignCert: X509_sign failed");
        }
    }

    // assign return value:
    if (result)
    {
        *signed_certificate = new_cert;
    }
    else
    {
      // cleanup in case of error to avoid memory leak
      if (new_cert != nullptr)
      {
          X509_free(new_cert);
          new_cert = nullptr;
      }
    }
    // cleanup:
    if (signing_request_key != nullptr)
    {
        EVP_PKEY_free(signing_request_key);
        signing_request_key = nullptr;
    }
    if (signer_certificate != nullptr)
    {
        X509_free(signer_certificate);
        signer_certificate= nullptr;
    }
    if (signer_key != nullptr)
    {
        EVP_PKEY_free(signer_key);
        signer_key = nullptr;
    }
    if ( result)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SignCert: succeeded");
    }
    return result;
}

} //volvo_on_call
/** \}    end of addtogroup */
