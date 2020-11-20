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
 *  \file     role_selected_notification_transaction.h
 *  \brief    Device pairing role selected transaction.
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup VolvoOnCall
 *  \{
 */

#include "signals/signal_types.h"
#include "transactions/role_selected_notification_transaction.h"

#include <iomanip>
#include <sstream>

#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/asn1.h>
#include <openssl/x509v3.h>

#include "ble_manager_interface.hpp"
#include "dlt/dlt.h"
#include "keystore.h"
#include "voc_framework/signals/vehicle_comm_signal.h"

#include "voc.h"

#define UUID_LENGTH 16

DLT_IMPORT_CONTEXT(dlt_voc);

namespace volvo_on_call
{

fsm::UserRole RoleSelectedNotificationTransaction::role_ = fsm::UserRole::kUndefined;

RoleSelectedNotificationTransaction::RoleSelectedNotificationTransaction(std::shared_ptr<fsm::Signal> initial_signal) :
    fsm::SmartTransaction(kNew)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s", __FUNCTION__);

    StateMap state_map = {{kNew, {nullptr,                 // state function
                                  SignalFunctionMap(),     // signal function map
                                  {kAwaitingCsr, kDone}}}, // valid transitions
                          {kAwaitingCsr, {nullptr,                         // state function
                                          SignalFunctionMap(),             // signal function map
                                          {kAwaitingCatalogueResponse}}},  // valid transitions
                          {kAwaitingCatalogueResponse, {nullptr,                    // state function
                                                        SignalFunctionMap(),   // signal function map
                                                        {kDone}}},               // valid transitions
                          {kDone, {nullptr,             // state function
                                   SignalFunctionMap(), // signal function map
                                   {}}}};               // valid transitions

    state_map[kNew].signal_function_map[fsm::Signal::kRoleSelectedNotify] =
        std::bind(&RoleSelectedNotificationTransaction::HandleRoleSelectedNotify,
                  this,
                  std::placeholders::_1);

    state_map[kAwaitingCsr].signal_function_map[VocSignalTypes::kCaDel002Signal] =
        std::bind(&RoleSelectedNotificationTransaction::HandleCaDel002,
                  this,
                  std::placeholders::_1);

    state_map[kAwaitingCsr].signal_function_map[VocSignalTypes::kCaDel010Signal] =
        std::bind(&RoleSelectedNotificationTransaction::HandleCaDel010,
                  this,
                  std::placeholders::_1);

    state_map[kAwaitingCatalogueResponse].signal_function_map[VocSignalTypes::kCaCat002Signal] =
        std::bind(&RoleSelectedNotificationTransaction::HandleCaCat002,
                  this,
                  std::placeholders::_1);


    SetStateMap(state_map);

    if (initial_signal)
    {
        MapTransactionId(initial_signal->GetTransactionId().GetSharedCopy());
        EnqueueSignal(initial_signal);
    }
}

fsm::UserRole RoleSelectedNotificationTransaction::GetSelectedRole()
{
    return role_;
}

std::string RoleSelectedNotificationTransaction::GenerateUUID()
{
    unsigned char uuid[UUID_LENGTH];
    std::string uuid_str;

    if (1 == RAND_bytes(uuid, UUID_LENGTH))
    {
        // 2. Adjust certain bits according to RFC 4122 section 4.4.
        // This just means do the following
        // (a) set the high nibble of the 7th byte equal to 4 and
        // (b) set the two most significant bits of the 9th byte to 10'B,
        //     so the high nibble will be one of {8,9,A,B}.
        uuid[6] = 0x40 | (uuid[6] & 0xf);
        uuid[8] = 0x80 | (uuid[8] & 0x3f);

        std::stringstream ss;

        // Set the stream's format to hex
        ss << std::hex;
        // Set the stream's fill character to '0' (for padding)
        ss << std::setfill('0');

        for(int i=0; i<UUID_LENGTH; ++i)
        {
            // The format for UUID is XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
            // add hyphens
            if (i == 4 || i == 6 || i == 8 || i == 10)
            {
                ss << '-';
            }

            // std::setw(2) sets the field width
            ss << std::setw(2) << static_cast<unsigned int>(uuid[i]);
        }

        uuid_str = ss.str();
    }

    return uuid_str;
}

EVP_PKEY* RoleSelectedNotificationTransaction::GenerateECKey()
{
    bool result = true;

    // Create the context for generating EC key parameters
    EVP_PKEY_CTX *param_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);

    if (!param_ctx)
    {
        result = false;
    }

    if (result)
    {
        if (1 != EVP_PKEY_paramgen_init(param_ctx))
        {
            result = false;
        }
    }

    // TODO (Dariusz): switch to correct curve Curve25519 after switch to newer OpenSSL version
    if (result)
    {
        if (1 != EVP_PKEY_CTX_set_ec_paramgen_curve_nid(param_ctx, NID_X9_62_prime256v1))
        {
            result = false;
        }
    }

    // Generate the parameters
    EVP_PKEY *parameters = NULL;

    if (result)
    {
        parameters = EVP_PKEY_new();

        if (!parameters)
        {
            result = false;
        }
    }

    if (result)
    {
        if (1 != EVP_PKEY_paramgen(param_ctx, &parameters))
        {
            result = false;
        }
    }

    // Generate the key using parameters generated above
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pkey_ctx = NULL;

    if (result)
    {
        pkey_ctx = EVP_PKEY_CTX_new(parameters, NULL);

        if (!pkey_ctx)
        {
            result = false;
        }
    }

    if (result)
    {
        if (1 != EVP_PKEY_keygen_init(pkey_ctx))
        {
            result = false;
        }
    }

    if (result)
    {
        if (1 != EVP_PKEY_keygen(pkey_ctx, &pkey))
        {
            result = false;
        }
        else
        {
            EC_KEY* ec_key = EVP_PKEY_get1_EC_KEY(pkey);
            if (ec_key)
            {
                EC_KEY_set_asn1_flag(ec_key, OPENSSL_EC_NAMED_CURVE);
            }
            else
            {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_WARN, "%s Failed to set asn1_flag on ec key.",
                                __FUNCTION__);
            }
        }
    }

    if (pkey_ctx)
    {
        EVP_PKEY_CTX_free(pkey_ctx);
    }

    if (parameters)
    {
        EVP_PKEY_free(parameters);
    }

    if (param_ctx)
    {
        EVP_PKEY_CTX_free(param_ctx);
    }

    return pkey;
}


bool RoleSelectedNotificationTransaction::GenerateVehicleCA()
{
    bool result = true;

    // Initialize a X509 structure for the certificate
    X509 *x509 = NULL;

    if (result)
    {
        x509 = X509_new();
        if (!x509)
        {
            result = false;
        }
    }

    // Get pointer to internal X509_NAME struct (don't free it as it's internal)
    X509_NAME *name = X509_get_subject_name(x509);

    if (!name)
    {
        result = false;
    }

    // Generate and set the Common Name (UUID)
    if (result)
    {
        std::string uuid = GenerateUUID();

        if (uuid.empty())
        {
            result = false;
        }

        if (result)
        {
            if (result && !X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                                                      reinterpret_cast<unsigned char*>(const_cast<char*>(uuid.c_str())),
                                                      -1, -1, 0))
            {
                result = false;
            }
        }
    }

    // Set the DC
    if (result)
    {
        std::string domain_component = "Volvo Cars";

        if (!X509_NAME_add_entry_by_txt(name, "DC", MBSTRING_ASC,
                                        reinterpret_cast<unsigned char*>(const_cast<char*>(domain_component.c_str())),
                                        -1, -1, 0))
        {
            result = false;
        }
        else
        {
            domain_component = "Car Root";

            if (!X509_NAME_add_entry_by_txt(name, "DC", MBSTRING_ASC,
                                            reinterpret_cast<unsigned char*>(const_cast<char*>(domain_component.c_str())),
                                            -1, -1, 0))
            {
                result = false;
            }
        }
    }

    // Set the UID
    if (result)
    {
        std::string user_identifier = "TCAM";

        if (!X509_NAME_add_entry_by_txt(name, "UID", MBSTRING_ASC,
                                        reinterpret_cast<unsigned char*>(const_cast<char*>(user_identifier.c_str())),
                                        -1, -1, 0))
        {
            result = false;
        }
    }

    // Set issuer and subject name to name
    if (result)
    {
        if (!X509_set_issuer_name(x509, name))
        {
            result = false;
        }
        else
        {
            if (!X509_set_subject_name(x509, name))
            {
                result = false;
            }
        }
    }

    // Create the EC key pair
    EVP_PKEY *pkey = GenerateECKey();

    if (!pkey)
    {
        result = false;
    }

    // Set public key
    if (0 == X509_set_pubkey(x509, pkey))
    {
        result = false;
    }

    // Set the key usage extension
    if (result)
    {
        if (!AddExtension(x509, x509, NID_key_usage, const_cast<char*>("critical,keyCertSign,cRLSign")))
        {
            result = false;
        }
    }

    // Set the Subject Key Identifier extension
    if (result)
    {
        if (!AddExtension(x509, x509, NID_subject_key_identifier, const_cast<char*>("hash")))
        {
            result = false;
        }
    }

    // Set the policy OID extension
    if (result)
    {
        if (!AddExtension(x509, x509, NID_certificate_policies, const_cast<char*>("1.3.6.1.4.1.37916.5.1.1")))
        {
            result = false;
        }
    }

    // Set serial number
    if (result)
    {
        result = SetSerialNumber(x509);
    }

    // Set valid-from and valid-to
    const long thirty_years_in_seconds = 30*365*24*60*60;

    if (result && !X509_gmtime_adj(X509_get_notBefore(x509), 0))
    {
        result = false;
    }

    if (result && !X509_gmtime_adj(X509_get_notAfter(x509), thirty_years_in_seconds))
    {
        result = false;
    }

    // Set certificate version
    if (!X509_set_version(x509, 2)) //v3
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s Failed to set certificate version",
                        __FUNCTION__);
         result = false;
    }

    // Sign the certificate
    if (result)
    {
        if (0 == X509_sign(x509, pkey, EVP_sha1()))
        {
            result = false;
        }
    }


    // Add the certificate to Keystore
    if (result)
    {
        fsm::CertificateId cert_id;

        fsm::ReturnCode ret_code = keystore_.AddCertificate(cert_id, x509,
                                                            fsm::Keystore::CertificateRole::kVehicleCa);

        if (ret_code != fsm::ReturnCode::kSuccess)
        {
            result = false;
        }
    }

    // Add the private key to Keystore
    if (result)
    {
        fsm::KeyId key_id;

        fsm::ReturnCode ret_code = keystore_.SetKey(key_id, NULL, pkey,
                                                    fsm::Keystore::KeyRole::kVehicleCa);

        if (ret_code != fsm::ReturnCode::kSuccess)
        {
            result = false;
        }
    }

    if (!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s Failed to generate CA root certificate",
                __FUNCTION__);
    }

    if (pkey)
    {
        EVP_PKEY_free(pkey);
    }

    if (x509)
    {
        X509_free(x509);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : generate Car Root CA self-signed certificate : %s", result?"OK":"failed" );
    return result;
}

bool RoleSelectedNotificationTransaction::GenerateVehicleEndPointCertificate(fsm::Keystore::CertificateRole role)
{
    bool result = true;

    // Initialize a X509 structure for the certificate
    X509 *x509 = NULL;

    if (result)
    {
        x509 = X509_new();
        if (!x509)
        {
            result = false;
        }
    }

    // Get the vehicle CA to use it as issuer of vehicle actor certificate and to
    // re-use it's common name
    STACK_OF(X509) *certificates = sk_X509_new_null();
    X509 *vehicle_ca = nullptr;

    if (result)
    {
        fsm::ReturnCode ret = keystore_.GetCertificates(certificates,
                                        fsm::Keystore::CertificateRole::kVehicleCa);

        // There should be only one CA certificate
        if (ret != fsm::ReturnCode::kSuccess || sk_X509_num(certificates) != 1)
        {
            result = false;
        }
        else
        {
            vehicle_ca = sk_X509_pop(certificates);
        }
    }

    // Get pointer to x509's name struct (don't free it as it's internal)
    X509_NAME *name = X509_get_subject_name(x509);

    if (!name)
    {
        result = false;
    }

    // Set the Common Name (should be same as vehicle CA's common name)
    if (result)
    {
        // Reserve space for UUID_LENGTH * 2 (each byte is in hex format XX) + 4 (hyphens in UUID)
        const int common_name_length = (UUID_LENGTH * 2) + 4;

        char common_name[common_name_length + 1];  // one extra for terminating nul character

        // Extract common name from vehicle_ca
        int ret = X509_NAME_get_text_by_NID(X509_get_subject_name(vehicle_ca),
                                            NID_commonName,
                                            common_name,
                                            common_name_length + 1);  // ditto

        if (ret != common_name_length)
        {
            result = false;
        }
        else if (!X509_NAME_add_entry_by_txt(name,
                                             "CN",
                                             MBSTRING_ASC,
                                             reinterpret_cast<unsigned char*>(common_name),
                                             -1, -1, 0))
        {
            result = false;
        }
    }

    // Set the DC
    if (result)
    {
        std::string domain_component = "Volvo Cars";

        if (!X509_NAME_add_entry_by_txt(name, "DC", MBSTRING_ASC,
                    reinterpret_cast<unsigned char*>(const_cast<char*>(domain_component.c_str())), -1, -1, 0))
        {
            result = false;
        }
        else
        {
            switch(role)
            {
            case fsm::Keystore::CertificateRole::kVehicleActor:
                domain_component = "Car Data Signing";
                break;
            case fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer:
                domain_component = "Car Nearfield Server";
                break;
            default:
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s(): Unknown certificate role.", __FUNCTION__);
                result = false;
                break;
            }

            if (result && !X509_NAME_add_entry_by_txt(name, "DC", MBSTRING_ASC,
                        reinterpret_cast<unsigned char*>(const_cast<char*>(domain_component.c_str())), -1, -1, 0))
            {
                result = false;
            }
        }
    }

    // Set subject name
    if (result)
    {
        if (!X509_set_subject_name(x509, name))
        {
            result = false;
        }
    }

    // Set the UID
    if (result)
    {
        std::string user_identifier = "TCAM";

        if (!X509_NAME_add_entry_by_txt(name, "UID", MBSTRING_ASC, reinterpret_cast<unsigned char*>(const_cast<char*>(user_identifier.c_str())), -1, -1, 0))
        {
            result = false;
        }
    }

    // Set issuer name
    if (result)
    {
        if (!X509_set_issuer_name(x509, X509_get_subject_name(vehicle_ca)))
        {
            result = false;
        }
    }

    // Create the EC key pair
    EVP_PKEY *pkey = GenerateECKey();

    if (!pkey)
    {
        result = false;
    }

    // Set public key
    if (0 == X509_set_pubkey(x509, pkey))
    {
        result = false;
    }

    // Set the key usage extension
    if (result)
    {
        std::string key_usage;

        switch(role)
        {
        case fsm::Keystore::CertificateRole::kVehicleActor:
            key_usage = "critical,digitalSignature,keyEncipherment";
            break;
        case fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer:
            key_usage = "critical,digitalSignature,keyEncipherment,keyAgreement";
            break;
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s(): Unknown certificate role.", __FUNCTION__);
            result = false;
            break;
        }

        if (result && !AddExtension(x509, x509, NID_key_usage, const_cast<char*>(key_usage.c_str())))
        {
            result = false;
        }
    }

    // Set the Subject Key Identifier extension
    if (result)
    {
        if (!AddExtension(x509, x509, NID_subject_key_identifier, const_cast<char*>("hash")))
        {
            result = false;
        }
    }

    // Set the policy OID extension
    if (result)
    {
        std::string policy_oid;

        switch(role)
        {
        case fsm::Keystore::CertificateRole::kVehicleActor:
            policy_oid = "1.3.6.1.4.1.37916.5.1.3";
            break;
        case fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer:
            policy_oid = "1.3.6.1.4.1.37916.5.1.2";
            break;
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s(): Unknown certificate role.", __FUNCTION__);
            result = false;
            break;
        }

        if (!AddExtension(x509, x509, NID_certificate_policies, const_cast<char*>(policy_oid.c_str())))
        {
            result = false;
        }
    }

    // Set serial number
    if (result)
    {
        result = SetSerialNumber(x509);
    }

    // Set valid-from and valid-to
    const long two_years_in_seconds = 2*365*24*60*60;

    if (result && !X509_gmtime_adj(X509_get_notBefore(x509), 0))
    {
        result = false;
    }

    if (result && !X509_gmtime_adj(X509_get_notAfter(x509), two_years_in_seconds))
    {
        result = false;
    }

    // Set certificate version
    if (!X509_set_version(x509, 2)) //v3
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s Failed to set certificate version",
                        __FUNCTION__);
         result = false;
    }

    // Sign the certificate
    if (result)
    {
        EVP_PKEY *ca_key = nullptr;

        fsm::ReturnCode ret_code = keystore_.GetKey(&ca_key,
                                                    fsm::Keystore::KeyRole::kVehicleCa);

        if (ret_code != fsm::ReturnCode::kSuccess || !ca_key)
        {
            result = false;
        }
        else
        {
            if (0 == X509_sign(x509, ca_key, EVP_sha1()))
            {
                result = false;
            }
        }

        if (ca_key)
        {
            EVP_PKEY_free(ca_key);
        }
    }

    // Add the certificate to Keystore
    if (result)
    {
        fsm::CertificateId cert_id;

        fsm::ReturnCode ret_code = keystore_.SetCertificate(cert_id, nullptr, x509, role);

        if (ret_code != fsm::ReturnCode::kSuccess)
        {
            result = false;
        }
    }

    // Add the private key to Keystore
    if (result)
    {
        fsm::KeyId key_id;
        fsm::Keystore::KeyRole key_role = fsm::Keystore::KeyRole::kUndefined;

        switch (role)
        {
        case fsm::Keystore::CertificateRole::kVehicleActor:
            key_role = fsm::Keystore::KeyRole::kVehicleActor;
            break;
        case fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer:
            key_role = fsm::Keystore::KeyRole::kVehicleShortRangeTlsServer;
            break;
        default:
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "%s(): Unknown certificate role.", __FUNCTION__);
            result = false;
            break;
        }

        if (result)
        {
            fsm::ReturnCode ret_code = keystore_.SetKey(key_id, NULL, pkey, key_role);

            if (ret_code != fsm::ReturnCode::kSuccess)
            {
                result = false;
            }
        }
    }

    if (!result)
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s Failed to generate vehicle certificate",
                __FUNCTION__);
    }

    if (pkey)
    {
        EVP_PKEY_free(pkey);
    }

    if (vehicle_ca)
    {
        X509_free (vehicle_ca);
    }

    if (certificates)
    {
        sk_X509_free(certificates);
    }

    if (x509)
    {
        X509_free(x509);
    }

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_OTP : generate Car Nearfield Server and Car Actor Certificate (and sign by Car Root CA certificate) : %s", result?"OK":"failed" );
    return result;
}

bool RoleSelectedNotificationTransaction::HandleRoleSelectedNotify(std::shared_ptr<fsm::Signal> signal)
{
    bool result = true;

    // TODO:
    // If not exists generate vehicel Actor
    // If not done start delegate creation
    //
    // Current stub impl emulates creation of a delegate

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // Save role:
    std::shared_ptr<fsm::RoleSelectedSignal> role_selected_signal = std::dynamic_pointer_cast<fsm::RoleSelectedSignal>(signal);
    if (role_selected_signal != nullptr)
    {
        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: entered Activity provideDevicePairing");
        std::shared_ptr<fsm::ResRoleSelectedNotify> rrsn = role_selected_signal->GetData();
        if ( (rrsn != NULL) && (rrsn->role_admin) )
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: simulated received pairingRole UserRole::kAdmin");
            role_ = fsm::UserRole::kAdmin;
        }
        else
        {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: simulated received pairingRole UserRole::kUser");
            role_ = fsm::UserRole::kUser;
        }

        // Add vehicle CA certificate if it's not yet in Keystore
        fsm::ReturnCode return_code = keystore_.GetCertificates(nullptr,
                                                                fsm::Keystore::CertificateRole::kVehicleCa);

        if (return_code == fsm::ReturnCode::kNotFound)
        {
            if (!GenerateVehicleCA())
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to generate CA.");
                result = false;
            }

            // Create a new vehicle actor
            if (result && !GenerateVehicleEndPointCertificate(fsm::Keystore::CertificateRole::kVehicleActor))
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to generate Vehicle Actor certificate.");
                result = false;
            }

            // Create a new vehicle TLS server
            if (result && !GenerateVehicleEndPointCertificate(fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer))
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to generate Vehicle TLS Server certificate.");
                result = false;
            }

            // Generate a new BDAK
            if (result)
            {
                BLEM::IPairingService* ble_pairing_service = BLEM::IBLEManager::GetPairingServiceInterface();

                if (ble_pairing_service)
                {
                    if (BLEM::BLEM_OK != ble_pairing_service->GenerateBDAK())
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_VUC : trigger creation of TCAM AES-128 key : failed");
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Failed to generate BDAK.");
                        result = false;
                    }
                    else
                    {
                        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "SIT: SW_APP --> SW_VUC : trigger creation of TCAM AES-128 key : OK");
                    }
                }
            }
        }
        else if (return_code == fsm::ReturnCode::kSuccess)
        {
            // Vehicle CA found, there should be Actor and TLS Server certs present as well
            return_code = keystore_.GetCertificate(nullptr,
                                                   fsm::Keystore::CertificateRole::kVehicleActor);

            if (return_code != fsm::ReturnCode::kSuccess)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Error - found Vehicle CA certificate but not Vehicle Actor.");
                result = false;
            }

            return_code = keystore_.GetCertificate(nullptr,
                                                   fsm::Keystore::CertificateRole::kVehicleShortRangeTlsServer);

            if (return_code != fsm::ReturnCode::kSuccess)
            {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "Error - found Vehicle CA certificate but not Vehicle TLS Server.");
                result = false;
            }

        }
        else
        {
            result = false;
        }

        if (!result)
        {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s Failed to generate vehicle certificates",
                    __FUNCTION__);
        }

        if (result)
        {
            SetState(kAwaitingCsr);
        }
    }
    return result;
}

bool RoleSelectedNotificationTransaction::HandleCaDel002(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;

    // TODO:
    // Request for delegate CSR failed, retry?
    //
    // Current stub impl emulates creation of a delegate

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    return return_value;
}

bool RoleSelectedNotificationTransaction::HandleCaDel010(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;

    // TODO:
    // 1. Sign CSR and send back CA-DEL-011
    // 2. send certificaet to catalogue, CA-CAT-001
    //
    // Current stub impl emulates creation of a delegate

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    SetState(kAwaitingCatalogueResponse);

    return return_value;
}

bool RoleSelectedNotificationTransaction::HandleCaCat002(std::shared_ptr<fsm::Signal> signal)
{
    bool return_value = true;

    // TODO:
    // If error retry?
    // Current stub impl emulates creation of a delegate

    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "%s received signal: %s in state %d",
                    __FUNCTION__, signal->ToString().c_str(), GetState());

    // transaction done
    SetState(kDone);
    return_value = false;

    return return_value;
}

} // namespace volvo_on_call
/** \}    end of addtogroup */
