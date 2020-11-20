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
 *  \file     test_classes.cc
 *  \brief    Volvo On Test Classes
 *  \author   Piotr Tomaszewski
 *
 *  \addtogroup TestVolvoOnCall
 *  \{
 */

#include "test_classes.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <cstring>
#include <vector>

#include "keystore.h"
#include <openssl/ssl.h>
#include "usermanager_interface.h"
#include "signals/ca_dpa_001_signal.h"
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(ut);

namespace fsm
{

// Here follows some certificates and code to load them
// what we are doig here is mocking the FoundationServiceManager
// certificate_store so we can control it
//
// Remember that currently everything involving keys is a hack pending
// proper fsm user management

//TODO: these are copied from Iulians ccm unittest in FSM
//      make common ut utilities package somehow?

X509* ut_GetCertFromPEM(const char* pPemData)
{
    X509* pCert = NULL;
    BIO* pCertBio = NULL;

    pCertBio = BIO_new(BIO_s_mem());

    if (pCertBio)
    {
        const size_t pemDataLength = strlen(pPemData);
        int writtenDataLength = BIO_write(pCertBio,
                                          reinterpret_cast<const void*>(pPemData),
                                          pemDataLength);

        // BIO must contain the full data inside the PEM
        if (writtenDataLength == pemDataLength)
        {
            pCert = PEM_read_bio_X509_AUX(pCertBio, NULL, NULL, NULL);
        }
    }

    return pCert;
}

X509_REQ* ut_GetCsrFromPEM(const char* pPemData)
{
    X509_REQ* pReq = NULL;
    BIO* pReqBio = NULL;

    pReqBio = BIO_new(BIO_s_mem());

    if (pReqBio)
    {
        const size_t pemDataLength = strlen(pPemData);
        int writtenDataLength = BIO_write(pReqBio,
                                          reinterpret_cast<const void*>(pPemData),
                                          pemDataLength);

        // BIO must contain the full data inside the PEM
        if (writtenDataLength == pemDataLength)
        {
            pReq = PEM_read_bio_X509_REQ(pReqBio, NULL, NULL, NULL);
        }
    }

    return pReq;
}

EVP_PKEY* ut_GetKeyFromPEM(const char* pPemData)
{
    EVP_PKEY* pKey = NULL;
    BIO* pKeyBio = NULL;

    pKeyBio = BIO_new(BIO_s_mem());

    if (pKeyBio)
    {
        const size_t pemDataLength = strlen(pPemData);
        int writtenDataLength = BIO_write(pKeyBio,
                                          reinterpret_cast<const void*>(pPemData),
                                          pemDataLength);

        if (writtenDataLength == pemDataLength)
        {
            pKey = PEM_read_bio_PrivateKey(pKeyBio, NULL, NULL, NULL);
        }
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

//TODO: there must be better ways of doing this
bool X509_REQ_eq(X509_REQ* a, X509_REQ* b)
{
    unsigned char* buffer_a = NULL;
    unsigned char* buffer_b = NULL;
    int buffer_length_a = 0;
    int buffer_length_b = -1; //let sensure thet are initialized as not eq.

    if (!a || !b)
    {
        // perhaps null and null should return true
        // but this is test support and we want to find issues
        // thus treating it as not eq seems better.
        return false;
    }

    if (a == b)
    {
        return true;
    }

    buffer_length_a = i2d_X509_REQ(a, &buffer_a);
    buffer_length_b = i2d_X509_REQ(b, &buffer_b);

    if (buffer_length_a != buffer_length_b)
    {
        return false;
    }

    if (std::memcmp(buffer_a, buffer_b, buffer_length_a) != 0)
    {
        return false;
    }

    return true;
}

// here comes a simple keystore mock

struct key_cert_pair
{
    X509 *cert;
    EVP_PKEY *key;
};


static struct key_cert_pair volatile vehicle_ca;
static struct key_cert_pair volatile vehicle_actor;
static struct key_cert_pair volatile vehicle_tls;

void SetCertsForEncode()
{
    user_cert = &CLIENT_ONE_CERT_PEM;
    user_key = &CLIENT_ONE_KEY_PEM;
    vehicle_actor.cert = ut_GetCertFromPEM(CLIENT_TWO_CERT_PEM.c_str());
    vehicle_actor.key = ut_GetKeyFromPEM(CLIENT_TWO_KEY_PEM.c_str());
}

void SetCertsForDecode()
{
    user_cert = &CLIENT_TWO_CERT_PEM;
    user_key = &CLIENT_TWO_KEY_PEM;
    vehicle_actor.cert = ut_GetCertFromPEM(CLIENT_ONE_CERT_PEM.c_str());
    vehicle_actor.key = ut_GetKeyFromPEM(CLIENT_ONE_KEY_PEM.c_str());
}

void SetDefaultCerts()
{
    vehicle_ca.cert = ut_GetCertFromPEM(ROOT_CERT_PEM.c_str());
    vehicle_ca.key = ut_GetKeyFromPEM(ROOT_KEY_PEM.c_str());
    user_cert = &CLIENT_ONE_CERT_PEM;
    user_key = &CLIENT_ONE_KEY_PEM;
    vehicle_actor.cert = ut_GetCertFromPEM(CLIENT_TWO_CERT_PEM.c_str());
    vehicle_actor.key = ut_GetKeyFromPEM(CLIENT_TWO_KEY_PEM.c_str());
}
}

namespace user_manager
{
Keystore::Keystore()
{
}

Keystore::~Keystore()
{
}

ReturnCode  Keystore::GetKnownSignerCertificates(STACK_OF(X509)* certificates)
{
    ReturnCode ret_code = ReturnCode::kSuccess;
    sk_X509_push(certificates, fsm::ut_GetCertFromPEM(fsm::user_cert->c_str()));
    return ret_code;
}

ReturnCode Keystore::AddCertificate(CertificateId& id,
                                    X509* certificate,
                                    const Keystore::CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kError;
    if (certificate)
    {
        switch(role)
        {
        case Keystore::CertificateRole::kVehicleCa:
            if (!fsm::vehicle_ca.cert)
            {
                fsm::vehicle_ca.cert = certificate;
                CRYPTO_add(&(fsm::vehicle_ca.cert->references), 1, CRYPTO_LOCK_X509);
                ret_code = ReturnCode::kSuccess;
            }
            break;
        default:
            ret_code = ReturnCode::kNotImplemented;
            std::cerr << "CertificateRole " << static_cast<int>(role) << " not implemented!" << std::endl;
            break;
        }
    }

    return ret_code;
}

ReturnCode Keystore::SetCertificate(CertificateId& id,
                                    X509** old_certificate,
                                    X509* certificate,
                                    const Keystore::CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kError;
    if (certificate)
    {
        switch(role)
        {
        case Keystore::CertificateRole::kVehicleActor:
            if (fsm::vehicle_actor.cert)
            {
                if (old_certificate)
                {
                    *old_certificate = fsm::vehicle_actor.cert;
                }
                else
                {
                    X509_free(fsm::vehicle_actor.cert);
                }
            }
            fsm::vehicle_actor.cert = certificate;
            CRYPTO_add(&(fsm::vehicle_actor.cert->references), 1, CRYPTO_LOCK_X509);
            ret_code = ReturnCode::kSuccess;
            break;

        case Keystore::CertificateRole::kVehicleShortRangeTlsServer:
            if (fsm::vehicle_tls.cert)
            {
                if (old_certificate)
                {
                    *old_certificate = fsm::vehicle_tls.cert;
                }
                else
                {
                    X509_free(fsm::vehicle_tls.cert);
                }
            }
            fsm::vehicle_tls.cert = certificate;
            CRYPTO_add(&(fsm::vehicle_tls.cert->references), 1, CRYPTO_LOCK_X509);
            ret_code = ReturnCode::kSuccess;
            break;

        default:
            ret_code = ReturnCode::kNotImplemented;
            std::cerr << "CertificateRole " << static_cast<int>(role) << " not implemented!" << std::endl;
            break;
        }
    }

    return ret_code;
}

ReturnCode Keystore::DeleteCertificate(X509** old_cert, const CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    switch(role)
    {
    case CertificateRole::kVehicleActor:
        if (fsm::vehicle_actor.cert)
        {
            if (old_cert)
            {
                *old_cert = fsm::vehicle_actor.cert;
            }
            else
            {
                X509_free(fsm::vehicle_actor.cert);
            }
            fsm::vehicle_actor.cert = NULL;
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }
        break;
    case CertificateRole::kVehicleShortRangeTlsServer:
        if (fsm::vehicle_tls.cert)
        {
            if (old_cert)
            {
                *old_cert = fsm::vehicle_tls.cert;
            }
            else
            {
                X509_free(fsm::vehicle_tls.cert);
            }
            fsm::vehicle_tls.cert = NULL;
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }
        break;
    default:
        ret_code = ReturnCode::kInvalidArgument;
        break;
    }

    return ret_code;
}

ReturnCode Keystore::DeleteCertificates(STACK_OF(X509)* old_certs, const CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    switch(role)
    {
    case CertificateRole::kVehicleCa:
        if (fsm::vehicle_ca.cert)
        {
            if (old_certs)
            {
                sk_X509_push(old_certs, fsm::vehicle_ca.cert);
            }
            else
            {
                X509_free(fsm::vehicle_ca.cert);
            }
            fsm::vehicle_ca.cert = NULL;
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }
        break;
    default:
        ret_code = ReturnCode::kInvalidArgument;
        break;
    }

    return ret_code;
}

ReturnCode  Keystore::GetCertificate(X509** certificate, const CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;
    const char* pPemData = nullptr;

    // we expect a nullptr or a valid pointer to nullptr.
    if (certificate && *certificate)
    {
        ret_code = ReturnCode::kInvalidArgument;
    }

    if (ret_code == ReturnCode::kSuccess)
    {
        switch(role)
        {
        case CertificateRole::kVehicleActor:
        case CertificateRole::kCloudSignEncrypt:
            if (fsm::vehicle_actor.cert)
            {
                if (certificate)
                {
                    *certificate = fsm::vehicle_actor.cert;
                    CRYPTO_add(&(fsm::vehicle_actor.cert->references), 1, CRYPTO_LOCK_X509);
                }
            }
            else
            {
                ret_code = ReturnCode::kNotFound;
            }
            break;
        case CertificateRole::kVehicleShortRangeTlsServer:
            if (fsm::vehicle_tls.cert)
            {
                if (certificate)
                {
                    *certificate = fsm::vehicle_tls.cert;
                    CRYPTO_add(&(fsm::vehicle_tls.cert->references), 1, CRYPTO_LOCK_X509);
                }
            }
            else
            {
                ret_code = ReturnCode::kNotFound;
            }
            break;
        case CertificateRole::kUserActor:
            pPemData = fsm::user_cert->c_str();
            break;
        default:
            ret_code = ReturnCode::kNotImplemented;
            std::cerr << "CertificateRole " << static_cast<int>(role) << " not implemented!" << std::endl;
            break;
        }
    }

    if (pPemData != nullptr)
    {
        X509 *result = fsm::ut_GetCertFromPEM(pPemData);
        if (!result)
        {
            ret_code = ReturnCode::kError;
        }
        else
        {
            if (certificate)
            {
                *certificate = result;
            }
        }
    }

    return ret_code;
}

ReturnCode Keystore::GetCertificates(STACK_OF(X509)* certificates, const CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    switch (role)
    {
    case CertificateRole::kVehicleCa:
        if (fsm::vehicle_ca.cert)
        {
            if (certificates)
            {
                sk_X509_push(certificates, fsm::vehicle_ca.cert);
                CRYPTO_add(&(fsm::vehicle_ca.cert->references), 1, CRYPTO_LOCK_X509);
            }
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }
        break;
    default:
        ret_code = ReturnCode::kNotImplemented;
        break;
    }

    return ret_code;
}

ReturnCode Keystore::SetKey(KeyId& id,
                            EVP_PKEY** old_key,
                            EVP_PKEY* key,
                            const Keystore::KeyRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    // we expect a valid pointer to nullptr, or nullptr pointer.
    if (old_key && *old_key)
    {
        ret_code = ReturnCode::kInvalidArgument;
    }
    if (!key)
    {
        ret_code = ReturnCode::kInvalidArgument;
    }

    if (ret_code == ReturnCode::kSuccess)
    {
        switch(role)
        {
        case KeyRole::kVehicleCa:
            if (fsm::vehicle_ca.key)
            {
                if (old_key)
                {
                    *old_key = fsm::vehicle_ca.key;
                }
                else
                {
                    EVP_PKEY_free(fsm::vehicle_ca.key);
                }
            }
            fsm::vehicle_ca.key = key;
            CRYPTO_add(&(fsm::vehicle_ca.key->references), 1, CRYPTO_LOCK_EVP_PKEY);
            break;
        case KeyRole::kVehicleActor:
            if (fsm::vehicle_actor.key)
            {
                if (old_key)
                {
                    *old_key = fsm::vehicle_actor.key;
                }
                else
                {
                    EVP_PKEY_free(fsm::vehicle_actor.key);
                }
            }
            fsm::vehicle_actor.key = key;
            CRYPTO_add(&(fsm::vehicle_actor.key->references), 1, CRYPTO_LOCK_EVP_PKEY);
            break;
        case KeyRole::kVehicleShortRangeTlsServer:
            if (fsm::vehicle_tls.key)
            {
                if (old_key)
                {
                    *old_key = fsm::vehicle_tls.key;
                }
                else
                {
                    EVP_PKEY_free(fsm::vehicle_tls.key);
                }
            }
            fsm::vehicle_tls.key = key;
            CRYPTO_add(&(fsm::vehicle_tls.key->references), 1, CRYPTO_LOCK_EVP_PKEY);
            break;
        default:
            ret_code = ReturnCode::kNotImplemented;
            std::cerr << "KeyRole " << static_cast<int>(role) << " not implemented!" << std::endl;
            break;
        }

    }

    return ret_code;
}

ReturnCode  Keystore::GetKey(EVP_PKEY** key, KeyRole role)
{
    ReturnCode ret_code = ReturnCode::kError;
    const char* pPemData = nullptr;
    switch(role)
    {
    case KeyRole::kVehicleActor:
    case KeyRole::kCloudSignEncrypt:
        if (fsm::vehicle_actor.key)
        {
            if (key)
            {
                *key = fsm::vehicle_actor.key;
                CRYPTO_add(&(fsm::vehicle_actor.key->references), 1, CRYPTO_LOCK_EVP_PKEY);
            }
            ret_code = ReturnCode::kSuccess;
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }
        break;
    case KeyRole::kVehicleCa:
        if (fsm::vehicle_ca.key)
        {
            if (key)
            {
                *key = fsm::vehicle_ca.key;
                CRYPTO_add(&(fsm::vehicle_ca.key->references), 1, CRYPTO_LOCK_EVP_PKEY);
            }
            ret_code = ReturnCode::kSuccess;
        }
        else
        {
            ret_code = ReturnCode::kNotFound;
        }
        break;
    default:
        ret_code = ReturnCode::kNotImplemented;
        std::cerr << "KeyRole " << static_cast<int>(role) << " not implemented!" << std::endl;
        break;
    }
    if (pPemData != nullptr)
    {
        *key = fsm::ut_GetKeyFromPEM(pPemData);
        if (*key != NULL)
        {
            ret_code = ReturnCode::kSuccess;
        }
    }
    return ret_code;
}

ReturnCode  Keystore::GetCertificateAuthorities(X509_STORE** ca_store)
{
    ReturnCode ret_code = ReturnCode::kError;
    X509* certs[1];
    certs[0] = fsm::ut_GetCertFromPEM(fsm::ROOT_CERT_PEM.c_str());
    *ca_store = fsm::ut_GetCertStore(certs, 1);
    if (*ca_store != NULL)
    {
        ret_code = ReturnCode::kSuccess;
    }
    return ret_code;
}

// here comes a simple usermanager mock

class UsermanagerInterfaceTestImpl : public UsermanagerInterface
{
    ReturnCode GetUser(UserId& id, const X509* certificate)
    {
        id = voc_test_classes::client_user_id;
        return ReturnCode::kSuccess;
    }

    ReturnCode GetUsers(std::vector<UserId>& ids, const UserRole role)
    {
        ids.push_back(voc_test_classes::client_user_id);
        return ReturnCode::kSuccess;
    }

    ReturnCode GetUsers(std::vector<UserId>& ids)
    {
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetUser(UserId& id, const std::vector<unsigned char>& common_name)
    {
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetActorCertificate(X509** certificate, UserId id)
    {
        *certificate = fsm::ut_GetCertFromPEM(fsm::user_cert->c_str());
        return ReturnCode::kSuccess;
    }

    ReturnCode GetShortRangeTlsClientCertificate(X509** certificate, const UserId id)
    {
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetActorCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids)
    {
        fsm::Keystore keystore;

        return keystore.GetKnownSignerCertificates(certificates) ==
            fsm::ReturnCode::kSuccess ?
            ReturnCode::kSuccess : ReturnCode::kError;
    }

    ReturnCode GetShortRangeTlsClientCertificates(STACK_OF(X509)* certificates, const std::vector<UserId> ids)
    {
        return ReturnCode::kNotImplemented;
    }

    ReturnCode GetUserRole(UserRole& role, const UserId user)
    {
        role = UserRole::kUser;

        return ReturnCode::kSuccess;
    }

    ReturnCode GetCommonName(std::vector<unsigned char>& common_name, const UserId id)
    {
        return ReturnCode::kNotImplemented;
    }

    ReturnCode AddUser(UserId& id, const UserRole role, X509* actor_certificate, X509* shortrange_tls_client_certificate)
    {
        DLT_LOG_STRINGF(ut, DLT_LOG_WARN, "UsermanagerInterfaceTestImpl::%s() NOT IMPLEMENTED returning default success", __func__);
        return ReturnCode::kSuccess;
    }

    ReturnCode DeleteUser(const UserId id)
    {
        return ReturnCode::kNotImplemented;
    }
};

std::shared_ptr<UsermanagerInterface> UsermanagerInterface::Create(std::shared_ptr<Keystore> keystore)
{
    return std::make_shared<UsermanagerInterfaceTestImpl>();
}

UsermanagerInterface::~UsermanagerInterface()
{
    // Clean up
}

} // namespace user_manager

#include "signals/basic_car_control_signal.h"
#include "signals/bcc_cl_001_signal.h"
#include "signals/bcc_cl_002_signal.h"
#include "signals/bcc_cl_010_signal.h"
#include "signals/bcc_cl_011_signal.h"
#include "signals/bcc_cl_020_signal.h"
#include "signals/ca_dpa_021_signal.h"


#include "voc_framework/signals/signal_factory.h"
#include "voc_framework/signals/ccm_encodable.h"
#include "voc_framework/signals/ccm_decodable.h"
#include "voc_framework/signals/ccm.h"

namespace voc_test_classes
{

using namespace volvo_on_call;

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

void DumpMessageFile(std::string name, const std::vector<unsigned char>& data)
{
    DumpMessageFile(name, (char*) &(data[0]), data.size());
}

void DumpPubKey(std::string name, EVP_PKEY* key)
{
    if(nullptr == key)
    return;
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, key);

    char* data = nullptr;
    size_t data_size = BIO_get_mem_data(bio, &data);
    DumpMessageFile(name, data, data_size);
    BIO_free(bio);
}

 /**
  * \brief Perform a ECDHE and provide the generated keys
  *        and shared secret.
  *
  * \return True on success, false on failure.
  */
bool MakeKeysAndSecret(EVP_PKEY** tcam_key,
                       EVP_PKEY** peer_key,
                       std::vector<unsigned char>& secret)
{
    EVP_PKEY_CTX* context = NULL;
    EVP_PKEY* params_tcam = NULL;
    EVP_PKEY* params_peer = NULL;

    // set up params
    context = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!context)
        return false;
    if (!EVP_PKEY_paramgen_init(context))
        return false;
    // TODO: use correct curve (needs openssl update or manual creation
    if (!EVP_PKEY_CTX_set_ec_paramgen_curve_nid(context, NID_X9_62_prime256v1))
        return false;
    if (!EVP_PKEY_paramgen(context, &params_tcam))
        return false;
    if (!EVP_PKEY_paramgen(context, &params_peer))
        return false;
    EVP_PKEY_CTX_free(context);

    // generate keys
    context = EVP_PKEY_CTX_new(params_tcam, NULL);
    if (!context)
        return false;
    if (!EVP_PKEY_keygen_init(context))
        return false;
    if (!EVP_PKEY_keygen(context, tcam_key))
        return false;
    EVP_PKEY_CTX_free(context);
    EVP_PKEY_free(params_tcam);

    context = EVP_PKEY_CTX_new(params_peer, NULL);
    if (!context)
        return false;
    if (!EVP_PKEY_keygen_init(context))
        return false;
    if (!EVP_PKEY_keygen(context, peer_key))
        return false;
    EVP_PKEY_CTX_free(context);
    EVP_PKEY_free(params_peer);

    // calculate shared secret
    size_t secret_length = 0;
    unsigned char* secret_buffer = nullptr;

    context = EVP_PKEY_CTX_new(*tcam_key, NULL);
    if (!EVP_PKEY_derive_init(context))
        return false;
    if (!EVP_PKEY_derive_set_peer(context, *peer_key))
        return false;
    if (!EVP_PKEY_derive(context, NULL, &secret_length))
        return false;
    secret_buffer = (unsigned char*) OPENSSL_malloc(secret_length);
    if (!secret_buffer)
        return false;
    if (!EVP_PKEY_derive(context, secret_buffer, &secret_length))
        return false;
    EVP_PKEY_CTX_free(context);

    secret.assign(secret_buffer, secret_buffer + secret_length);

    OPENSSL_free(secret_buffer);

    return true;
}


bool TimeEqual(struct tm *tm1, struct tm *tm2)
{
    time_t t1 = mktime(tm1);
    time_t t2 = mktime(tm2);
    double diffSecs = difftime(t1, t2);
    return (diffSecs==0);
}


/****************************************************
 * BCC CL 001
 * *************************************************/
void* TestBccCl001Signal::GetPackedPayload()
{
    BCC_CL_001_t* asn1c_bcc_cl_001 = (BCC_CL_001_t*) calloc(1, sizeof(BCC_CL_001_t));

    switch (supportedAdvert)
    {
    case Advert::Honk :
        asn1c_bcc_cl_001->operation.present = operation_PR_locationAdvert;
        asn1c_bcc_cl_001->operation.choice.locationAdvert.advert = Adverts_honk;
        break;

    case Advert::Flash :
        asn1c_bcc_cl_001->operation.present = operation_PR_locationAdvert;
        asn1c_bcc_cl_001->operation.choice.locationAdvert.advert = Adverts_flash;
        break;

    case Advert::HonkAndFlash :
        asn1c_bcc_cl_001->operation.present = operation_PR_locationAdvert;
        asn1c_bcc_cl_001->operation.choice.locationAdvert.advert = Adverts_honkAndFlash;
        break;

    default: return NULL;

    }

    return (void*) asn1c_bcc_cl_001;
};

//setting of the Advert type
void TestBccCl001Signal::SetAdvert(Advert advert) {supportedAdvert = advert;};



asn_TYPE_descriptor_t* TestBccCl001Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_001;
};

const char* TestBccCl001Signal::GetOid ()
{
    return oid.c_str();
};


fs_VersionInfo TestBccCl001Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
};

//constructor
TestBccCl001Signal::TestBccCl001Signal (fsm::CCMTransactionId& transaction_id,
                                        uint16_t sequence_number) : BccCl001Signal::BccCl001Signal(transaction_id,
                                                                                                   sequence_number)
{

};


//Factory function for the test signal
std::shared_ptr<TestBccCl001Signal> TestBccCl001Signal::CreateTestBccCl001Signal(
        fsm::CCMTransactionId& transaction_id,
        uint16_t sequence_number)
{
    TestBccCl001Signal* signal = new TestBccCl001Signal(transaction_id,
                                                        sequence_number);

    return std::shared_ptr<TestBccCl001Signal>(signal);
};
//TestBccCl001Signal


/****************************************************
 * BCC CL 002
 * *************************************************/

std::shared_ptr<TestBccCl002Signal> TestBccCl002Signal::CreateTestBccCl002Signal(ccm_Message* ccm,
                                                                                 fsm::TransactionId& transaction_id)
{
    TestBccCl002Signal* signal = new TestBccCl002Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<TestBccCl002Signal>();
    }
    else
    {
        return std::shared_ptr<TestBccCl002Signal>(signal);
    }
}


TestBccCl002Signal::TestBccCl002Signal (ccm_Message* ccm,
                                        fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
    BccCl002Signal::BccCl002Signal(ccm,
                                   transaction_id)
{
    //asn1c struct to hold decoded data
    BCC_CL_002_t* asn1c_bcc_CL_002 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_bcc_CL_002, &asn_DEF_BCC_CL_002, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_bcc_CL_002);
    }

    // free decoded asn1c struct
    asn_DEF_BCC_CL_002.free_struct(&asn_DEF_BCC_CL_002, asn1c_bcc_CL_002, 0);
}

bool TestBccCl002Signal::UnpackPayload(BCC_CL_002_t* asn1c_bcc_CL_002)
{
    bool return_value = true;

    //check if resources are available
    if (asn1c_bcc_CL_002)
    {
        return_value = UnpackData(&(asn1c_bcc_CL_002->responseInfo));
    }
    else
    {
        return_value = false;
    }

    return return_value;
}



/****************************************************
 * BCC CL 010
 ****************************************************/

void* TestBccCl010Signal::GetPackedPayload()
{
    return calloc(1, sizeof(BCC_CL_010_t));
};

asn_TYPE_descriptor_t* TestBccCl010Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_BCC_CL_010;
};

const char* TestBccCl010Signal::GetOid ()
{
    return oid.c_str();
};

fs_Encoding TestBccCl010Signal::GetPreferedEncoding ()
{
    // seems that ASN1C can not UPER encoide empty sequences
    return FS_ENCODING_DER;
}


fs_VersionInfo TestBccCl010Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
};

//constructor
TestBccCl010Signal::TestBccCl010Signal (fsm::CCMTransactionId& transaction_id,
                                        uint16_t sequence_number) : BccCl010Signal::BccCl010Signal(transaction_id,
                                                                                                   sequence_number)
{};


//Factory function for the test signal
std::shared_ptr<TestBccCl010Signal> TestBccCl010Signal::CreateTestBccCl010Signal(
        fsm::CCMTransactionId& transaction_id,
        uint16_t sequence_number)
{
    TestBccCl010Signal* signal = new TestBccCl010Signal(transaction_id,
                                                        sequence_number);

    return std::shared_ptr<TestBccCl010Signal>(signal);
};



/****************************************************
 * BCC CL 011
 ****************************************************/
//TODO: Class is partly going to look weird, this is because
//      its largely copy paste from bcc_cl_020 and bcc_cl_002.
//      Redo when refactoring signals.
std::shared_ptr<TestBccCl011Signal> TestBccCl011Signal::CreateTestBccCl011Signal(ccm_Message* ccm,
                                                                                 fsm::TransactionId& transaction_id)
{
    TestBccCl011Signal* signal = new TestBccCl011Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<TestBccCl011Signal>();
    }
    else
    {
        return std::shared_ptr<TestBccCl011Signal>(signal);
    }
}

TestBccCl011Signal::TestBccCl011Signal (ccm_Message* ccm,
                                        fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                         BccCl011Signal::BccCl011Signal(ccm,
                                                                                                        transaction_id)
{
    //asn1c struct to hold decoded data
    BCC_CL_011_t* asn1c_bcc_cl_011 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_bcc_cl_011, &asn_DEF_BCC_CL_011, GetContentVersion());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_bcc_cl_011);
    }

    // free decoded asn1c struct
    asn_DEF_BCC_CL_011.free_struct(&asn_DEF_BCC_CL_011, asn1c_bcc_cl_011, 0);
}

bool TestBccCl011Signal::UnpackPayload(BCC_CL_011_t* asn1c_bcc_cl_011)
{
    bool return_value = true;

    //check if resources are available
    if (asn1c_bcc_cl_011)
    {
        // first call response info supercalss to unapck response info
        return_value = UnpackData(&(asn1c_bcc_cl_011->responseInfo));


        // Now do optional location
        if (return_value && asn1c_bcc_cl_011->location)
        {
            switch (asn1c_bcc_cl_011->location->present)
            {
            case location_PR_minimumLocation :
                return_value =
                    UnpackMinimumLocation(&(asn1c_bcc_cl_011->location->choice.minimumLocation));
                break;
            case location_PR_extendedLocation :
                return_value =
                    UnpackExtendedLocation(&(asn1c_bcc_cl_011->location->choice.extendedLocation));
                break;
            case location_PR_NOTHING :
            default :
                return_value = true; //location is optional
            }
        }
    }
    else
    {
        return_value = false;
    }

    return return_value;
}

bool TestBccCl011Signal::CompareLocationData(std::shared_ptr<BccCl011Signal> signal_a)
{
    bool return_value = true;

    std::shared_ptr<fsm::LocationData> location_data_a;
    location_data_a = std::dynamic_pointer_cast<fsm::LocationData>(signal_a);

    fsm::LocationData* location_data_b = dynamic_cast<fsm::LocationData*>(this);

    if (!location_data_a || !location_data_b)
    {
        return_value = false;
    }
    else
    {
        return_value = *location_data_a == *location_data_b;
    }
    return return_value;
}


/****************************************************
* BCC CL 020
* *************************************************/

std::shared_ptr<TestBccCl020Signal> TestBccCl020Signal::CreateTestBccCl020Signal(ccm_Message* ccm,
                                                                                 fsm::TransactionId& transaction_id)
{
    TestBccCl020Signal* signal = new TestBccCl020Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<TestBccCl020Signal>();
    }
    else
    {
        return std::shared_ptr<TestBccCl020Signal>(signal);
    }
}

TestBccCl020Signal::TestBccCl020Signal (ccm_Message* ccm,
                                        fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                         BccCl020Signal::BccCl020Signal(ccm,
                                                                                                        transaction_id)
{
    //asn1c struct to hold decoded data
    BCC_CL_020_t* asn1c_bcc_cl_020 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_bcc_cl_020, &asn_DEF_BCC_CL_020, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_bcc_cl_020);
    }

    // free decoded asn1c struct
    asn_DEF_BCC_CL_020.free_struct(&asn_DEF_BCC_CL_020, asn1c_bcc_cl_020, 0);
}

bool TestBccCl020Signal::UnpackPayload(BCC_CL_020_t* asn1c_bcc_cl_020)
{
    bool return_value = false;

    //check if resources are available
    if (asn1c_bcc_cl_020)
    {
        switch (asn1c_bcc_cl_020->location.present)
        {
        case location_PR_minimumLocation :
            return_value =
                    UnpackMinimumLocation(&(asn1c_bcc_cl_020->location.choice.minimumLocation));
            break;
        case location_PR_extendedLocation :
            return_value =
                    UnpackExtendedLocation(&(asn1c_bcc_cl_020->location.choice.extendedLocation));
            break;
        case location_PR_NOTHING :
        default :
            return_value = false;
        }

    }

    return return_value;
}

ccm_storeAndForwardOptions TestBccCl020Signal::GetStoreAndForwardOptions()
{
    return message->header.delivery_options.store_and_forward;
}

bool TestBccCl020Signal::CompareLocationData(std::shared_ptr<BccCl020Signal> signal_a)
{
    bool return_value = true;

    std::shared_ptr<fsm::LocationData> location_data_a;
    location_data_a = std::dynamic_pointer_cast<fsm::LocationData>(signal_a);

    fsm::LocationData* location_data_b = dynamic_cast<fsm::LocationData*>(this);

    if (!location_data_a || !location_data_b)
    {
        return_value = false;
    }
    else
    {
        return_value = *location_data_a == *location_data_b;
    }
    return return_value;
}


/****************************************************
* CA CAT 001
* *************************************************/

std::shared_ptr<TestCaCat001Signal> TestCaCat001Signal::CreateTestCaCat001Signal(ccm_Message* ccm,
                                                                                 fsm::TransactionId& transaction_id)
{
    TestCaCat001Signal* signal = new TestCaCat001Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<TestCaCat001Signal>();
    }
    else
    {
        return std::shared_ptr<TestCaCat001Signal>(signal);
    }
}

TestCaCat001Signal::TestCaCat001Signal (ccm_Message* ccm,
                                        fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                         CaCat001Signal::CaCat001Signal(ccm,
                                                                                                        transaction_id)
{

    //asn1c struct to hold decoded data
    CA_CAT_001_t* asn1c_ca_cat_001 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_ca_cat_001, &asn_DEF_CA_CAT_001, GetContentVersion ());

    if (valid)
    {
        // extract the data and store in class members
        valid = UnpackPayload(asn1c_ca_cat_001);
    }

    // free decoded asn1c struct
    asn_DEF_CA_CAT_001.free_struct(&asn_DEF_CA_CAT_001, asn1c_ca_cat_001, 0);

}

bool TestCaCat001Signal::UnpackPayload(CA_CAT_001_t* asn1c_ca_cat_001)
{
    bool return_value = true;
    //check if resources are available
    if (asn1c_ca_cat_001)
    {
        unsigned int num_certs = asn1c_ca_cat_001->certificates.list.count;

        for (unsigned int i = 0; i < num_certs && return_value; i++)
        {

            //extract single certificate
            struct Certificate *cert = asn1c_ca_cat_001->certificates.list.array[i];

            //encode it to der so that openssl can open it
            unsigned char *der_encoded_cert = (unsigned char*)OPENSSL_malloc(10000);
            unsigned char *der_encoded_cert_copy = der_encoded_cert;
            asn_enc_rval_t result = der_encode_to_buffer(&asn_DEF_Certificate,
                                                         cert,
                                                         der_encoded_cert_copy,
                                                         10000);
            if (result.encoded == -1)
            {
                //failed to encode to der, returning error
                return_value = false;
            }
            else
            {

                //convert it to X509* for later comparison
                X509* received_cert = d2i_X509(NULL, (const unsigned char **)&der_encoded_cert_copy, result.encoded);

                //store it
                received_certs_.push_back(received_cert);

                OPENSSL_free(der_encoded_cert);
            }

        }

    }
    else
    {
        return_value = false;
    }

    return return_value;
}

TestCaCat001Signal::~TestCaCat001Signal()
{

    for (X509 * cert: received_certs_)
    {
        X509_free(cert);
    }
}

/****************************************************
 * CA CAT 002
 * *************************************************/
void* TestCaCat002Signal::GetPackedPayload()
{
    CA_CAT_002_t* asn1c_ca_cat_002 = (CA_CAT_002_t*) calloc(1, sizeof(CA_CAT_002_t));

    if (asn1c_ca_cat_002)
    {
        PackData(&(asn1c_ca_cat_002->responseInfo));
    }

    return (void*) asn1c_ca_cat_002;
}

asn_TYPE_descriptor_t* TestCaCat002Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_CA_CAT_002;
}

//constructor
TestCaCat002Signal::TestCaCat002Signal (fsm::CCMTransactionId& transaction_id,
                                          uint16_t sequence_number) : CaCat002Signal::CaCat002Signal(transaction_id,
                                                                                                     sequence_number)
{
}

//destructor
TestCaCat002Signal::~TestCaCat002Signal ()
{
}

const char* TestCaCat002Signal::GetOid ()
{
    return oid_.c_str();
}

fs_VersionInfo TestCaCat002Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}

//Factory function for the test signal
std::shared_ptr<TestCaCat002Signal> TestCaCat002Signal::CreateTestCaCat002Signal(fsm::CCMTransactionId& transaction_id,
                                                                                 uint16_t sequence_number)
{
    TestCaCat002Signal* signal = new TestCaCat002Signal(transaction_id, sequence_number);

    return std::shared_ptr<TestCaCat002Signal>(signal);
}

/****************************************************
* CA DEL 001
* *************************************************/

std::shared_ptr<TestCaDel001Signal> TestCaDel001Signal::CreateTestCaDel001Signal(ccm_Message* ccm,
                                                                                 fsm::TransactionId& transaction_id)
{
    TestCaDel001Signal* signal = new TestCaDel001Signal(ccm, transaction_id);
    if (!signal->valid)
    {
        delete signal;
        return std::shared_ptr<TestCaDel001Signal>();
    }
    else
    {
        return std::shared_ptr<TestCaDel001Signal>(signal);
    }
}

TestCaDel001Signal::TestCaDel001Signal (ccm_Message* ccm,
                                        fsm::TransactionId& transaction_id) : fsm::CCM::CCM(ccm),
                                                                         CaDel001Signal::CaDel001Signal(ccm,
                                                                                                        transaction_id)
{

    //asn1c struct to hold decoded data
    CA_DEL_001_t* asn1c_ca_del_001 = NULL;

    //call super decode with input data from here
    valid = DecodePayload((void**)&asn1c_ca_del_001, &asn_DEF_CA_DEL_001, GetContentVersion ());

    // free decoded asn1c struct
    asn_DEF_CA_DEL_001.free_struct(&asn_DEF_CA_DEL_001, asn1c_ca_del_001, 0);
}

/****************************************************
 * CA DEL 002
 * *************************************************/
void* TestCaDel002Signal::GetPackedPayload()
{
    CA_DEL_002_t* asn1c_ca_del_002 = (CA_DEL_002_t*) calloc(1, sizeof(CA_DEL_002_t));

    if (asn1c_ca_del_002)
    {
        PackData(&(asn1c_ca_del_002->responseInfo));
    }

    return (void*) asn1c_ca_del_002;
}

asn_TYPE_descriptor_t* TestCaDel002Signal::GetPayloadTypeDescriptor ()
{
    return &asn_DEF_CA_DEL_002;
}

//constructor
TestCaDel002Signal::TestCaDel002Signal (fsm::CCMTransactionId& transaction_id,
                                          uint16_t sequence_number) : CaDel002Signal::CaDel002Signal(transaction_id,
                                                                                                     sequence_number)
{
}

//destructor
TestCaDel002Signal::~TestCaDel002Signal ()
{
}

const char* TestCaDel002Signal::GetOid ()
{
    return oid_.c_str();
}

fs_VersionInfo TestCaDel002Signal::GetContentVersion ()
{
    fs_VersionInfo version_info;

    version_info.signal_flow_version = kSignalFlowVersion;
    version_info.schema_version = kSchemaVersion;
    version_info.preliminary_version = kPreliminaryVersion;

    return version_info;
}

//Factory function for the test signal
std::shared_ptr<TestCaDel002Signal> TestCaDel002Signal::CreateTestCaDel002Signal(fsm::CCMTransactionId& transaction_id,
                                                                                 uint16_t sequence_number)
{
    TestCaDel002Signal* signal = new TestCaDel002Signal(transaction_id, sequence_number);

    return std::shared_ptr<TestCaDel002Signal>(signal);
}

std::shared_ptr<fsm::Signal> CreateTestSignalFromCCM (char* binary_ccm, size_t num_bytes)
{
    std::shared_ptr<fsm::Signal> return_signal;

    ccm_Message* message = NULL;
    const_fs_Data encoded_data = {binary_ccm, num_bytes};

    fsm::UserId sender;

    if (!ccm_Decode(&encoded_data,
                    &message,
                    sender,
                    nullptr,
                    0))
    {
        return return_signal;
    }

    fsm::CCMTransactionId transaction_id(message->header.transaction_id);

    char* oid = message->payload.content.oid.oid;

    if (TestBccCl002Signal::oid.compare(oid) == 0)
    {
        return_signal = TestBccCl002Signal::CreateTestBccCl002Signal(message, transaction_id);
    }
    else if (TestBccCl011Signal::oid.compare(oid) == 0)
    {
        return_signal = TestBccCl011Signal::CreateTestBccCl011Signal(message, transaction_id);
    }
    else if (TestBccCl020Signal::oid.compare(oid) == 0)
    {
        return_signal = TestBccCl020Signal::CreateTestBccCl020Signal(message, transaction_id);
    }
    else if (TestCaCat001Signal::oid.compare(oid) == 0)
    {
        return_signal = TestCaCat001Signal::CreateTestCaCat001Signal(message, transaction_id);
    }
    else if (TestCaDel001Signal::oid.compare(oid) == 0)
    {
        return_signal = TestCaDel001Signal::CreateTestCaDel001Signal(message, transaction_id);
    }
    else if (CaDpa001Signal::oid_.compare(oid) == 0)
    {
        return_signal = CaDpa001Signal::CreateCaDpa001Signal(message, transaction_id);
    }
    else if (CaDpa021Signal::oid_.compare(oid) == 0)
    {
        return_signal = CaDpa021Signal::CreateCaDpa021Signal(message, transaction_id);
    }

    if (sender != fsm::kUndefinedUserId)
    {
        return_signal->SetSender(sender);
    }

    return return_signal;
}


} // namespace voc_test_classes

/** \}    end of addtogroup */
