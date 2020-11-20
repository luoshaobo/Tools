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
 *  \file     keystore_stub.cc
 *  \brief    Keystore stub implemenation
 *  \author   Imran Siddique
 *
 *  \addtogroup unittest
 *  \{
 */

#include "keystore_stub.h"

#include <iostream>
#include <string>
#include <vector>
#include <openssl/ssl.h>

#include "utilities.h"
#include "certUtils.h"

namespace fsm
{

struct key_cert_pair
{
    X509 *cert;
    EVP_PKEY *key;
};

static struct key_cert_pair volatile vehicle_actor;
static const std::string* volatile user_cert;

void GenerateCerts()
{
    vehicle_actor.cert = ut_GetCertFromPEM(CLIENT_TWO_CERT_PEM.c_str());
    vehicle_actor.key = ut_GetKeyFromPEM(CLIENT_TWO_KEY_PEM.c_str());
    user_cert = &CLIENT_TWO_CERT_PEM;
}

void FreeCerts()
{
    // Free Certificates
    X509_free(vehicle_actor.cert);

    // Free Keys
    EVP_PKEY_free(vehicle_actor.key);

    vehicle_actor.cert = nullptr;
    vehicle_actor.key = nullptr;

    user_cert = nullptr;
}
} // namespace fsm

namespace user_manager
{
ReturnCode Keystore::GetCertificate(X509** certificate, const CertificateId id)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    if (id == fsm::client_cert_id)
    {
        if (certificate)
        {
            *certificate = ut_GetCertFromPEM(fsm::user_cert->c_str());
        }
    }
    else
    {
        ret_code = ReturnCode::kNotFound;
    }

    return ret_code;
}

ReturnCode  Keystore::GetCertificate(X509** certificate, const CertificateRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    // we expect a nullptr or a valid pointer to nullptr.
    if (certificate && *certificate)
    {
        ret_code = ReturnCode::kInvalidArgument;
    }

    if (ret_code == ReturnCode::kSuccess)
    {
        switch(role)
        {
        case CertificateRole::kUndefined:
            ret_code = ReturnCode::kUndefined;
            break;
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
        case CertificateRole::kVehicleCa:
        case CertificateRole::kVehicleShortRangeTlsServer:
        case CertificateRole::kUserActor:
        case CertificateRole::kUserShortRangeTlsClient:
        case CertificateRole::kCloudCa:
        case CertificateRole::kCloudTlsClient:
        case CertificateRole::kDoIpTlsServer:
            ret_code =  ReturnCode::kNotImplemented;
            std::cerr << "CertificateRole " << static_cast<int>(role) << " not implemented!" << std::endl;
            break;
        default:
            ret_code = ReturnCode::kInvalidArgument;
            break;
        }
    }

    return ret_code;
}

ReturnCode Keystore::GetKey(EVP_PKEY** key, KeyRole role)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    // we expect a nullptr or a valid pointer to nullptr.
    if (key && *key)
    {
        ret_code = ReturnCode::kInvalidArgument;
    }

    if (ret_code == ReturnCode::kSuccess)
    {
        switch(role)
        {
        case KeyRole::kUndefined:
            ret_code = ReturnCode::kUndefined;
            break;
        case KeyRole::kVehicleActor:
        case KeyRole::kCloudSignEncrypt:
            if (fsm::vehicle_actor.key)
            {
                if (key)
                {
                    *key = fsm::vehicle_actor.key;
                    CRYPTO_add(&(fsm::vehicle_actor.key->references), 1, CRYPTO_LOCK_EVP_PKEY);
                }
            }
            else
            {
                ret_code = ReturnCode::kNotFound;
            }
            break;
        case KeyRole::kVehicleCa:
        case KeyRole::kVehicleShortRangeTlsServer:
        case KeyRole::kCloudTlsClient:
        case KeyRole::kDoIpTlsServer:
            ret_code = ReturnCode::kNotImplemented;
            std::cerr << "KeyRole " << static_cast<int>(role) << " not implemented!" << std::endl;
            break;
        default:
            ret_code = ReturnCode::kInvalidArgument;
            break;
        }
    }

    return ret_code;
}

ReturnCode  Keystore::GetCertificateAuthorities(X509_STORE** ca_store)
{
    ReturnCode ret_code = ReturnCode::kSuccess;

    // we expect a nullptr or a valid pointer to nullptr.
    if (ca_store && *ca_store)
    {
        ret_code = ReturnCode::kInvalidArgument;
    }

    if (ret_code == ReturnCode::kSuccess)
    {
        X509* certs[1];
        certs[0] = ut_GetCertFromPEM(fsm::ROOT_CERT_PEM.c_str());
        if (ca_store)
        {
            *ca_store = ut_GetCertStore(certs, 1);
        }
    }
    return ret_code;
}

ReturnCode  Keystore::GetKnownSignerCertificates(STACK_OF(X509)* certificates)
{
    ReturnCode ret_code = ReturnCode::kSuccess;
    if (fsm::user_cert)
    {
        sk_X509_push(certificates, ut_GetCertFromPEM(fsm::user_cert->c_str()));
    }
    else
    {
        ret_code = ReturnCode::kNotFound;
    }

    return ret_code;
}

Keystore::Keystore()
{
}

Keystore::~Keystore()
{
}

} // namespace user_manager
